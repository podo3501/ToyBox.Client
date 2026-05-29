#pragma once
#include "MaterialDesc.h"

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

enum class MeshTextureSlot : uint32_t
{
    Albedo,
    Normal,
    Count
};

struct MeshMaterialDesc : public MaterialDesc
{
    MaterialSurface surface;

    MeshMaterialDesc()
    {
        type = MaterialType::Mesh;
        pipelineState = PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default);
        surface = { 0.5f, 0.f };
    }

    bool operator==(const MeshMaterialDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            MaterialDesc::GetHash(),
            surface.GetHash());
    }
};