#include "pch.h"
#include "ShaderSystem.h"

ShaderSystem::~ShaderSystem() = default;
ShaderSystem::ShaderSystem() = default;

//SURFACE_DEBUG
//SPECULAR_DEBUG
//DIFFUSE_DEBUG
//D3D_SHADER_MACRO defines[] = { "", "1", nullptr, nullptr };

static bool CompileStage(
    const std::string& source, 
    const D3D_SHADER_MACRO* defines,
    const char* entry, 
    const char* target, 
    ComPtr<ID3DBlob>& outBlob)
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG;
    flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompile(
        source.data(),
        source.size(),
        nullptr,
        defines,
        nullptr,
        entry,
        target,
        flags,
        0,
        &outBlob,
        &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            const char* err = static_cast<const char*>(errorBlob->GetBufferPointer());
            OutputDebugStringA(err);
        }

        return false;
    }

    return true;
}

static ComPtr<ID3DBlob> CreateBlobFromBuffer(const Core::ByteBuffer& buffer)
{
    if (buffer.empty()) return nullptr;

    ComPtr<ID3DBlob> blob;
    if (FAILED(D3DCreateBlob(buffer.size(), &blob)))
        return nullptr;

    memcpy(blob->GetBufferPointer(), buffer.data(), buffer.size());

    return blob;
}

bool ShaderSystem::Register(ShaderID shaderID, std::shared_ptr<ShaderAsset> asset)
{
    if (!asset)
        return false;

    auto [srcIt, inserted] = m_sources.emplace(shaderID, std::move(asset));
    if (!inserted)
        return true;

    ShaderVariant baseVariant{ shaderID, ShaderDefine::None };

    ShaderEntry entry;
    if (!CompileVariant(baseVariant, *srcIt->second, entry))
        return false;

    m_variants.emplace(baseVariant, std::move(entry));

    return true;
}

const ShaderEntry* ShaderSystem::Find(const ShaderVariant& variant) const
{
    auto it = m_variants.find(variant);
    if (it != m_variants.end())
        return &it->second;

    auto srcIt = m_sources.find(variant.shaderID);
    if (srcIt == m_sources.end())
        return nullptr; // shader가 등록이 안돼 있다.

    ShaderEntry entry;
    if (!CompileVariant(variant, *srcIt->second, entry))
        return nullptr;

    auto [iter, inserted] = m_variants.emplace(variant, std::move(entry)); //lazy cache 이기 때문에 mutable 처리.

    return &iter->second;
}

static bool HasFlag(ShaderDefine value, ShaderDefine flag)
{
    return
        (static_cast<uint32_t>(value) &
            static_cast<uint32_t>(flag)) != 0;
}

bool ShaderSystem::CompileVariant(const ShaderVariant& variant, const ShaderAsset& asset, ShaderEntry& outEntry) const
{
    // HLSL source compile
    if (!asset.hlslSource.empty())
    {
        D3D_SHADER_MACRO defines[8]{};

        int idx = 0;

        if (HasFlag(variant.defines, ShaderDefine::DiffuseDebug)) defines[idx++] = { "DIFFUSE_DEBUG", "1" };
        if (HasFlag(variant.defines, ShaderDefine::SpecularDebug)) defines[idx++] = { "SPECULAR_DEBUG", "1" };
        if (HasFlag(variant.defines, ShaderDefine::SurfaceDebug)) defines[idx++] = { "SURFACE_DEBUG", "1" };

        defines[idx] = { nullptr, nullptr };

        CompileStage(asset.hlslSource, defines, "VSMain", "vs_5_0", outEntry.vs);
        CompileStage(asset.hlslSource, defines, "PSMain", "ps_5_0", outEntry.ps);
        //CompileStage(asset.hlslSource, defines, "CSMain", "cs_5_0", outEntry.cs); //아직 cs는 없다. 디버그창에 메세지가 떠서 일단은 주석처리
    }
    else
    {
        outEntry.vs = CreateBlobFromBuffer(asset.vs);
        outEntry.ps = CreateBlobFromBuffer(asset.ps);
        //outEntry.cs = CreateBlobFromBuffer(asset.cs); //아직 cs는 없다. 디버그창에 메세지가 떠서 일단은 주석처리
    }

    return outEntry.vs && outEntry.ps;
}