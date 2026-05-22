#include "pch.h"
#include "ShaderSystem.h"
#include "GameClient/Service/Asset/Assets/ShaderAsset.h"

ShaderSystem::~ShaderSystem() = default;
ShaderSystem::ShaderSystem() = default;

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

bool ShaderSystem::Initialize(const std::vector<ShaderRegisterDesc>& shaders)
{
    for (const ShaderRegisterDesc& desc : shaders)
    {
        if (!desc.asset)
            return false;

        ShaderData shaderData;
        shaderData.asset = desc.asset;
        shaderData.stages = desc.stages;

        auto [shaderIt, inserted] = m_shaders.emplace(desc.shaderID, std::move(shaderData));
        if (!inserted)
            continue;

        ShaderVariant baseVariant{ desc.shaderID };
        ShaderEntry entry;
        
        if (!CompileVariant(baseVariant, *shaderIt->second.asset, shaderIt->second.stages, entry))
            return false;

        m_variants.emplace(baseVariant, std::move(entry));
    }

    return true;
}

const ShaderEntry* ShaderSystem::Find(const ShaderVariant& variant) const
{
    auto it = m_variants.find(variant);
    if (it != m_variants.end())
        return &it->second;

    auto shaderIt = m_shaders.find(variant.shaderID);
    if (shaderIt == m_shaders.end())
        return nullptr; // shader가 등록이 안돼 있다.

    const ShaderData& shaderData = shaderIt->second;
    ShaderEntry entry;
    if (!CompileVariant(variant, *shaderData.asset, shaderData.stages, entry))
        return nullptr;

    auto [iter, inserted] = m_variants.emplace(variant, std::move(entry)); //lazy cache 이기 때문에 mutable 처리.

    return &iter->second;
}

bool ShaderSystem::CompileVariant(
    const ShaderVariant& variant,
    const ShaderAsset& asset,
    const std::vector<ShaderStageDesc>& stages,
    ShaderEntry& outEntry) const
{
    // precompiled shader
    if (asset.hlslSource.empty())
    {
        outEntry.vs = CreateBlobFromBuffer(asset.vs);
        outEntry.ps = CreateBlobFromBuffer(asset.ps);
        outEntry.cs = CreateBlobFromBuffer(asset.cs);

        return outEntry.vs || outEntry.ps || outEntry.cs;
    }

    bool compiledAny = false;
    for (const ShaderStageDesc& stage : stages)
    {
        D3D_SHADER_MACRO defines[32]{};

        int idx = 0;
        for (const ShaderMacroDesc& macro : variant.runtimeMacros)
            defines[idx++] = { macro.name.c_str(), macro.value.c_str() };
        defines[idx] = { nullptr, nullptr };

        ComPtr<ID3DBlob>* targetBlob = nullptr;
        switch (stage.stage)
        {
        case ShaderStage::Vertex: targetBlob = &outEntry.vs; break;
        case ShaderStage::Pixel: targetBlob = &outEntry.ps; break;
        case ShaderStage::Compute: targetBlob = &outEntry.cs; break;
        default: return false;
        }

        if (!CompileStage(asset.hlslSource, defines, stage.entry.c_str(), stage.target.c_str(), *targetBlob))
            return false;

        compiledAny = true;
    }

    return compiledAny;
}