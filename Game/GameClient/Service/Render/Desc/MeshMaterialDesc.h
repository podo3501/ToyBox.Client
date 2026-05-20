#pragma once
#include "MaterialDesc.h"
#include "TextureDesc.h"

struct TextureAsset;

struct MaterialSurface
{
    float roughness{ 0.5f };
    float metallic{ 0.0f };

    bool operator==(const MaterialSurface&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(roughness, metallic);
    }
};

struct MeshMaterialDesc : public MaterialDesc
{
    TextureDesc albedoDesc;
    MaterialSurface surface;

    MeshMaterialDesc()
    {
        type = MaterialType::Mesh;
        pipelineState = PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default);
        albedoDesc = { true, true };
        surface = { 0.5f, 0.f };
    }

    bool operator==(const MeshMaterialDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            MaterialDesc::GetHash(),
            albedoDesc.GetHash(),
            surface.GetHash());
    }
};