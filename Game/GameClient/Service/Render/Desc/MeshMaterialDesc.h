#pragma once
#include "MaterialDesc.h"

struct MaterialSurface
{
    float normalIntensity{ 1.f };
    float ambientOcclusionIntensity{ 1.f };
    float roughnessIntensity{ 0.5f };
    float metallic{ 0.0f };

    bool operator==(const MaterialSurface&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(normalIntensity, ambientOcclusionIntensity, roughnessIntensity, metallic);
    }
};

enum class MeshTextureSlot : uint32_t
{
    Albedo,
    Normal,
    ARM, //R: AO, G: Roughness, B: Metallic
    Count
};

struct MeshMaterialDesc : public MaterialDesc
{
    MaterialSurface surface{};

    MeshMaterialDesc()
    {
        type = MaterialType::Mesh;
        pipelineState = PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default);
    }

    bool operator==(const MeshMaterialDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            MaterialDesc::GetHash(),
            surface.GetHash());
    }
};