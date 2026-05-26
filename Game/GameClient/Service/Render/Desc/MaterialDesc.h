#pragma once
#include "RenderState.h"

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

    bool operator==(const MaterialDesc&) const = default;

    virtual size_t GetHash() const
    {
        return Core::HashOf(
            type, 
            pipelineState.GetHash());
    }
};