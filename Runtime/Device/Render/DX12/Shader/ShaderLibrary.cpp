#include "pch.h"
#include "ShaderLibrary.h"
#include "Core/Utils/StringUtils.h"
#include "GameClient/Asset/ShaderAsset.h"
#pragma comment(lib, "dxcompiler.lib")

ShaderLibrary::~ShaderLibrary() = default;
ShaderLibrary::ShaderLibrary()
{
    m_shaders.resize(1); //런타임이 먼저 들어올수 있기 때문. shaderKey 생성값은 size를 리턴하기 때문이다.
}

static bool CompileStage(
    const std::string& source, 
    const std::vector<ShaderMacroDesc>& runtimeMacros,
    const char* entry, 
    const char* target, 
    ComPtr<IDxcBlob>& outBlob)
{
    ComPtr<IDxcUtils> dxcUtils;
    ComPtr<IDxcCompiler3> dxcCompiler;

    if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils)))) return false;
    if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler)))) return false;

    DxcBuffer sourceBuffer{};
    sourceBuffer.Ptr = source.data();
    sourceBuffer.Size = source.size();
    sourceBuffer.Encoding = DXC_CP_UTF8; // ANSI/UTF-8 기본값

    std::vector<std::wstring> args; // 진입점(-E) 및 타겟 프로필(-T, 예: cs_6_6) 지정
    args.push_back(L"-E"); args.push_back(Core::ToWString(entry));
    args.push_back(L"-T"); args.push_back(Core::ToWString(target));

    // 디버그 플래그 및 최적화 설정
#if defined(_DEBUG)
    args.push_back(L"-Zi"); // 디버그 정보 포함
    args.push_back(L"-Od"); // 최적화 해제 (Skip Optimization)
#else
    args.push_back(L"-O3"); // 최대 최적화
#endif

    for (const auto& macro : runtimeMacros) // 런타임 매크로 인자 전개 (-D 이름=값)
    {
        if (macro.name.empty())
            continue;

        std::wstring macroStr = Core::ToWString(macro.name) + L"=" + Core::ToWString(macro.value);
        args.push_back(L"-D");
        args.push_back(macroStr);
    }

    // std::wstring 목록을 DXC 호환 LPCWSTR 배열로 캐스팅
    std::vector<LPCWSTR> dxcArgs;
    dxcArgs.reserve(args.size());
    for (const auto& arg : args)
        dxcArgs.push_back(arg.c_str());

    ComPtr<IDxcResult> compileResult;
    HRESULT hr = dxcCompiler->Compile(
        &sourceBuffer,
        dxcArgs.data(),
        static_cast<UINT32>(dxcArgs.size()),
        nullptr, // 빌드 환경에 Include Handler가 필요하다면 여기에 세팅
        IID_PPV_ARGS(&compileResult)
    );

    if (FAILED(hr)) return false;

    ComPtr<IDxcBlobUtf8> errorBlob;
    if (SUCCEEDED(compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorBlob), nullptr)) && errorBlob && errorBlob->GetStringLength() > 0)
        OutputDebugStringA(errorBlob->GetStringPointer());

    HRESULT compileStatus;
    if (FAILED(compileResult->GetStatus(&compileStatus)) || FAILED(compileStatus))
        return false;

    return SUCCEEDED(compileResult->GetResult(&outBlob));
}

static ComPtr<IDxcBlob> CreateBlobFromBuffer(const Core::ByteBuffer& buffer)
{
    if (buffer.empty()) return nullptr;

    ComPtr<IDxcUtils> dxcUtils;
    if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils))))
        return nullptr;

    ComPtr<IDxcBlobEncoding> blobEncoding;
    if (FAILED(dxcUtils->CreateBlob(buffer.data(), static_cast<UINT32>(buffer.size()), DXC_CP_ACP, &blobEncoding)))
        return nullptr;

    ComPtr<IDxcBlob> blob;
    if (SUCCEEDED(blobEncoding.As(&blob)))
    {
        return blob;
    }

    return nullptr;
}

bool ShaderLibrary::Initialize(std::span<const BuiltinShaderDesc> builtinShaders)
{
    for (const auto& [key, desc] : builtinShaders)
        ReturnIfFalse(RegisterShader(key, desc));
    
    return true;
}

bool ShaderLibrary::RegisterShader(ShaderKey key, const ShaderDesc& desc)
{
    if (key == InvalidShaderKey)
        return false;

    if (!desc.asset)
        return false;

    if (key >= m_shaders.size())
        m_shaders.resize(static_cast<size_t>(key) + 1);

    ShaderData& shaderData = m_shaders[key];

    if (shaderData.asset)
        return false;

    shaderData.asset = desc.asset;
    shaderData.stages = desc.stages;

    ShaderVariant baseVariant{ key };
    ShaderEntry entry;

    if (!CompileVariant(baseVariant, shaderData, entry))
        return false;

    m_variants.emplace(baseVariant, std::move(entry));

    return true;
}

ShaderKey ShaderLibrary::RegisterShader(const ShaderDesc& desc)
{
    ShaderKey key = static_cast<ShaderKey>(m_shaders.size());

    if (!RegisterShader(key, desc))
        return InvalidShaderKey;

    return key;
}

const ShaderEntry* ShaderLibrary::Find(const ShaderVariant& variant) const
{
    if (variant.shaderKey == InvalidShaderKey)
        return nullptr;

    auto it = m_variants.find(variant);
    if (it != m_variants.end())
        return &it->second;

    if (variant.shaderKey >= m_shaders.size())
        return nullptr;

    const ShaderData& shaderData = m_shaders[variant.shaderKey];
    if (!shaderData.asset) return nullptr; // shader가 등록이 안돼 있다.

    ShaderEntry entry;
    if (!CompileVariant(variant, shaderData, entry))
        return nullptr;

    auto [iter, inserted] = m_variants.emplace(variant, std::move(entry)); //lazy cache 이기 때문에 mutable 처리.

    return &iter->second;
}

bool ShaderLibrary::CompileVariant(
    const ShaderVariant& variant,
    const ShaderData& shaderData,
    ShaderEntry& outEntry) const
{
    auto& asset = *shaderData.asset;

    // precompiled shader
    if (asset.hlslSource.empty())
    {
        outEntry.vs = CreateBlobFromBuffer(asset.vs);
        outEntry.ps = CreateBlobFromBuffer(asset.ps);
        outEntry.cs = CreateBlobFromBuffer(asset.cs);

        return outEntry.vs || outEntry.ps || outEntry.cs;
    }

    bool compiledAny = false;
    for (const ShaderStageDesc& stage : shaderData.stages)
    {
        ComPtr<IDxcBlob>* targetBlob = nullptr;
        switch (stage.stage)
        {
        case ShaderStage::Vertex: targetBlob = &outEntry.vs; break;
        case ShaderStage::Pixel: targetBlob = &outEntry.ps; break;
        case ShaderStage::Compute: targetBlob = &outEntry.cs; break;
        default: return false;
        }

        if (!CompileStage(asset.hlslSource, variant.runtimeMacros, stage.entry.c_str(), stage.target.c_str(), *targetBlob))
            return false;

        compiledAny = true;
    }

    return compiledAny;
}