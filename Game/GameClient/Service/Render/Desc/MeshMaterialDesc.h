#pragma once
#include "MaterialDesc.h"
#include "TextureBinding.h"

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
    std::vector<TextureBinding> textures;
    MaterialSurface surface;

    MeshMaterialDesc()
    {
        textures.resize(static_cast<size_t>(TextureSlot::Count));

        type = MaterialType::Mesh;
        pipelineState = PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default);
        surface = { 0.5f, 0.f };
    }

    bool operator==(const MeshMaterialDesc&) const = default;

    size_t GetHash() const
    {
        size_t h = MaterialDesc::GetHash();
        for (const auto& tex : textures)
            Core::HashCombine(h, tex.GetHash());
        Core::HashCombine(h, surface.GetHash());

        return h;
    }
};