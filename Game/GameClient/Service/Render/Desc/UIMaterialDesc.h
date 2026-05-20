#pragma once
#include "MaterialDesc.h"
#include "TextureDesc.h"

struct UIMaterialDesc : public MaterialDesc
{
    TextureDesc texDesc;

    UIMaterialDesc()
    {
        type = MaterialType::UI;
        texDesc = { true, false };
        pipelineState = PipelineLibrary::Get(ShaderID::UI, RasterPreset::NoCull);
    }

    bool operator==(const UIMaterialDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            MaterialDesc::GetHash(),
            texDesc.GetHash());
    }
};