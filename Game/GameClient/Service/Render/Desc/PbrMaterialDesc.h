#pragma once
#include "SurfaceMaterialDesc.h"

enum class PbrTextureSlot : uint32_t
{
    Albedo,
    Normal,
    ARM, //R: AO, G: Roughness, B: Metallic
    Count
};

constexpr TextureSlot Resolve(PbrTextureSlot s)
{
    return static_cast<TextureSlot>(s);
}

struct PbrSurface
{
    float normal{ 1.f }; //세기값
    float ao{ 1.f }; //세기값
    float roughness{ 0.5f }; //1이 거친것
    float metallic{ 0.0f }; //1이 메탈릭

    bool operator==(const PbrSurface&) const = default;
    size_t GetHash() const { return Core::HashOf(normal, ao, roughness, metallic); }
};

struct PbrMaterialDesc : public SurfaceMaterialDesc
{
    PbrSurface surf{};

    PbrMaterialDesc()
    {
        surfType = SurfaceType::PBR;
        pipelineState = PipelineLibrary::Get(
            RegistryShader::PBR,
            RasterPreset::Default,
            PrimitiveTopologyType::Triangle);
    }

    bool operator==(const PbrMaterialDesc&) const = default;
    size_t GetHash() const { return Core::HashOf(SurfaceMaterialDesc::GetHash(), surf.GetHash()); }
};