#pragma once
#include "GameClient/Service/Render/Desc/RenderState.h"
#include <d3dcompiler.h>
#include <wrl.h>

struct ShaderAsset;

using Microsoft::WRL::ComPtr;

struct ShaderEntry
{
    Microsoft::WRL::ComPtr<ID3DBlob> vs;
    Microsoft::WRL::ComPtr<ID3DBlob> ps;
    Microsoft::WRL::ComPtr<ID3DBlob> cs;
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
    //bool CompileVariant(const ShaderVariant& variant, const ShaderAsset& asset, ShaderEntry& outEntry) const;
    bool CompileVariant(
        const ShaderVariant& variant,
        const ShaderAsset& asset,
        const std::vector<ShaderStageDesc>& stages,
        ShaderEntry& outEntry) const;

    std::unordered_map<ShaderID, ShaderData> m_shaders;
    mutable std::unordered_map<ShaderVariant, ShaderEntry, ShaderVariantHasher> m_variants;
};
