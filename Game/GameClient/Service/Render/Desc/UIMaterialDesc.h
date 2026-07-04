#pragma once
#include "MaterialDesc.h"
#include "TextureDesc.h"

enum class UITextureSlot : uint32_t
{
    Normal,
    Count
};

constexpr TextureSlot Resolve(UITextureSlot s)
{
    return static_cast<TextureSlot>(s);
}

struct UIMaterialDesc : public MaterialDesc
{
    UIMaterialDesc()
    {
        domain = MaterialDomain::UserInterface;
        pipelineState = PipelineLibrary::Get(
            RegistryShader::UI,
            RasterPreset::NoCull, 
            PrimitiveTopologyType::Triangle);
    }

    bool operator==(const UIMaterialDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            MaterialDesc::GetHash());
    }
};