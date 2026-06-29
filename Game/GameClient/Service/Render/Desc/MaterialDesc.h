#pragma once
#include "RenderState.h"
#include "TextureDesc.h"

enum class MaterialDomain
{
    Surface,
    DebugSurface,
    UserInterface,
    Count
};

struct MaterialDesc
{
    virtual ~MaterialDesc() = default;

    MaterialDomain domain{ MaterialDomain::Surface };
    PipelineState pipelineState { PipelineLibrary::Get(BuiltinShader::Phong, RasterPreset::Default) };
    std::unordered_map<TextureSlot, TextureDesc> textures;

    bool operator==(const MaterialDesc&) const = default;

    virtual size_t GetHash() const
    {
        size_t h;
        Core::HashCombine(h, domain);
        Core::HashCombine(h, pipelineState.GetHash());
        for (const auto& [slot, tex] : textures)
        {
            Core::HashCombine(h, slot);
            Core::HashCombine(h, tex.GetHash());
        }
        return h;
    }
};