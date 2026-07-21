#pragma once
#include "GameClient/Service/Render/Definition/RenderState.h"
#include <d3dcompiler.h>
#include <wrl.h>
#include <dxcapi.h>

struct ShaderAsset;

using Microsoft::WRL::ComPtr;

struct ShaderEntry
{
    Microsoft::WRL::ComPtr<IDxcBlob> vs;
    Microsoft::WRL::ComPtr<IDxcBlob> ps;
    Microsoft::WRL::ComPtr<IDxcBlob> cs;
};

struct ShaderData
{
    std::shared_ptr<ShaderAsset> asset{ nullptr };
    std::vector<ShaderStageDesc> stages;
};

class ShaderLibrary
{
public:
    ~ShaderLibrary();
    ShaderLibrary();
    bool Initialize(std::span<const RegistryShaderDesc> registryShaders);
    ShaderID RegisterShader(const ShaderDesc& desc);
    const ShaderEntry* Find(const ShaderVariant& variant) const;

private:
    bool RegisterShader(ShaderID id, const ShaderDesc& desc);
    bool CompileVariant(
        const ShaderVariant& variant,
        const ShaderData& shaderData,
        ShaderEntry& outEntry) const;

    std::vector<ShaderData> m_shaders;
    mutable std::unordered_map<ShaderVariant, ShaderEntry, ShaderVariantHasher> m_variants;
};
