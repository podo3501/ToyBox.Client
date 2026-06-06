#pragma once
#include "RenderState.h"
#include "TextureDesc.h"

enum class MaterialDomain
{
    Surface,
    UserInterface
};

struct MaterialDesc
{
    virtual ~MaterialDesc() = default;

    MaterialDomain domain{ MaterialDomain::Surface };
    PipelineState pipelineState { PipelineLibrary::Get(ShadingModel::PBR, RasterPreset::Default) };
    std::vector<TextureDesc> textures;

    bool operator==(const MaterialDesc&) const = default;

    virtual size_t GetHash() const
    {
        size_t h;
        Core::HashCombine(h, domain);
        Core::HashCombine(h, pipelineState.GetHash());
        for (const auto& tex : textures)
            Core::HashCombine(h, tex.GetHash());
        return h;
    }
};