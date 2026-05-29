#pragma once
#include "MaterialDesc.h"
#include "TextureDesc.h"

enum class UITextureSlot : uint32_t
{
    Normal,
    Count
};

struct UIMaterialDesc : public MaterialDesc
{
    UIMaterialDesc()
    {
        type = MaterialType::UI;
        pipelineState = PipelineLibrary::Get(ShaderID::UI, RasterPreset::NoCull);
    }

    bool operator==(const UIMaterialDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            MaterialDesc::GetHash());
    }
};