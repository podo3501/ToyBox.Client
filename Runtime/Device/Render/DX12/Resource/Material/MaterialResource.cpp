#include "pch.h"
#include "MaterialResource.h"

MaterialResource::~MaterialResource() = default;

PipelineState MaterialResource::GetPipelineState(
    const std::optional<RasterPreset>& rasterOverride,
    const std::optional<ShaderID>& shaderOverride) const
{
    PipelineState result = m_pipelineState;

    if (rasterOverride)
        result.rasterState = RasterLibrary::Get(*rasterOverride);

    if (shaderOverride)
        result.shaderVariant.shaderID = *shaderOverride;

    return result;
}