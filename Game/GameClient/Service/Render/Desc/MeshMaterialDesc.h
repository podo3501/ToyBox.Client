#pragma once
#include "MaterialDesc.h"

struct MaterialSurface
{
    float normalIntensity{ 1.f };
    float roughnessIntensity{ 1.f };
    float metallic{ 0.0f };

    bool operator==(const MaterialSurface&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(normalIntensity, roughnessIntensity, metallic);
    }
};

enum class MeshTextureSlot : uint32_t
{
    Albedo,
    Normal,
    Roughness,
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