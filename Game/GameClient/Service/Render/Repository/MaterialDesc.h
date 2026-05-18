#pragma once
#include <filesystem>
#include "TextureDesc.h"
#include "RenderState.h"

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

struct MaterialDesc
{
    TextureDesc albedoDesc;
    MaterialSurface surface;
    PipelineState pipelineState{ PipelineLibrary::Get(RasterPreset::Default) };

    bool operator==(const MaterialDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            albedoDesc.GetHash(),
            surface.GetHash(),
            pipelineState.GetHash());
    }
};