#pragma once
#include "MaterialDesc.h"
#include "TextureBinding.h"

struct UIMaterialDesc : public MaterialDesc
{
    std::vector<TextureBinding> textures;

    UIMaterialDesc()
    {
        type = MaterialType::UI;
        pipelineState = PipelineLibrary::Get(ShaderID::UI, RasterPreset::NoCull);
    }

    bool operator==(const UIMaterialDesc&) const = default;

    size_t GetHash() const
    {
        size_t h = MaterialDesc::GetHash();
        for (const auto& tex : textures)
            Core::HashCombine(h, tex.GetHash());

        return h;
    }
};