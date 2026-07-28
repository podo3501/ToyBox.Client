#pragma once
#include "Core/Foundation/ResourceID.h"
#include "../RenderState.h"

using TextureSlot = uint32_t;
enum class MaterialDomain
{
    Surface,
    DebugSurface,
    UserInterface,
    Count
};

struct TextureBinding
{
    TextureSlot slot;
    Core::ResourceID resource;
    TextureBinding(TextureSlot s, Core::ResourceID r) :
        slot{ s }, 
        resource{ std::move(r) }
    {}

    bool operator==(const TextureBinding&) const = default;
    size_t GetHash() const { return Core::HashOf(slot, resource); }
};


struct MaterialDesc
{
    virtual ~MaterialDesc() = default;

    MaterialDomain domain{ MaterialDomain::Surface };
    PipelineState pipelineState { PipelineLibrary::Get(RegistryShader::Phong, RasterPreset::Default) };
    std::vector<TextureBinding> textures;

    void SetShaderID(ShaderID shaderID)
    {
        pipelineState.shaderVariant.shaderID = shaderID;
    }

    bool operator==(const MaterialDesc&) const = default;

    virtual size_t GetHash() const
    {
        size_t h;
        Core::HashCombine(h, domain);
        Core::HashCombine(h, pipelineState.GetHash());
        for (auto& tex : textures)
            Core::HashCombine(h, tex.GetHash());
        return h;
    }
};