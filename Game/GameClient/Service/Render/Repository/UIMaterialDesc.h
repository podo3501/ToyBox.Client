#pragma once
#include "TextureDesc.h"
#include "RenderState.h"

struct UIMaterialDesc
{
    TextureDesc texDesc;
    PipelineState pipelineState{ PipelineLibrary::Get(ShaderID::UI, RasterPreset::NoCull) };

    bool operator==(const UIMaterialDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            texDesc.GetHash(),
            pipelineState.GetHash());
    }
};