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
        domain = MaterialDomain::UserInterface;
        pipelineState = PipelineLibrary::Get(
            ShadingModel::UI, 
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