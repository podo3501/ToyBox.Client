#pragma once
#include "RenderState.h"
#include "TextureDesc.h"

enum class MaterialType
{
    Mesh,
    UI
};

struct MaterialDesc
{
    virtual ~MaterialDesc() = default;

    MaterialType type{ MaterialType::Mesh };
    PipelineState pipelineState { PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default) };
    std::vector<TextureDesc> textures;

    bool operator==(const MaterialDesc&) const = default;

    virtual size_t GetHash() const
    {
        size_t h;
        Core::HashCombine(h, type);
        Core::HashCombine(h, pipelineState.GetHash());
        for (const auto& tex : textures)
            Core::HashCombine(h, tex.GetHash());
        return h;
    }
};