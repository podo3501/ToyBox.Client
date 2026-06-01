#pragma once
#include "GameClient/Service/Render/Desc/RenderState.h"
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
    std::shared_ptr<ShaderAsset> asset;
    std::vector<ShaderStageDesc> stages;
};

class ShaderSystem
{
public:
    ~ShaderSystem();
    ShaderSystem();
    bool Initialize(const std::vector<ShaderRegisterDesc>& shaders);
    
    const ShaderEntry* Find(const ShaderVariant& variant) const;

private:
    bool CompileVariant(
        const ShaderVariant& variant,
        const ShaderAsset& asset,
        const std::vector<ShaderStageDesc>& stages,
        ShaderEntry& outEntry) const;

    std::unordered_map<ShaderID, ShaderData> m_shaders;
    mutable std::unordered_map<ShaderVariant, ShaderEntry, ShaderVariantHasher> m_variants;
};
