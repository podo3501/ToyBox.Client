#pragma once
#include "GameClient/Service/Render/Repository/Shader/IShaderSystem.h"
#include "GameClient/Service/Render/Desc/RenderState.h"
#include <d3dcompiler.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

struct ShaderEntry
{
    Microsoft::WRL::ComPtr<ID3DBlob> vs;
    Microsoft::WRL::ComPtr<ID3DBlob> ps;
    Microsoft::WRL::ComPtr<ID3DBlob> cs;
};

class ShaderSystem : public IShaderSystem
{
public:
    ~ShaderSystem();
    ShaderSystem();

    virtual bool Register(ShaderID shaderID, std::shared_ptr<ShaderAsset> asset) override;

    const ShaderEntry* Find(const ShaderVariant& variant) const;

private:
    bool CompileVariant(const ShaderVariant& variant, const ShaderAsset& asset, ShaderEntry& outEntry) const;

    std::unordered_map<ShaderID, std::shared_ptr<ShaderAsset>> m_sources;
    mutable std::unordered_map<ShaderVariant, ShaderEntry, ShaderVariantHasher> m_variants;
};
