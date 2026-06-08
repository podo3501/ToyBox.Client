#pragma once
#include "DebugSurfaceMaterialDesc.h"

struct GridMaterialDesc : public DebugSurfaceMaterialDesc
{
    GridMaterialDesc()
    {
        debugSurfType = DebugSurfaceType::Grid;
        pipelineState = PipelineLibrary::Get(
            ShadingModel::Grid, 
            RasterPreset::Default, 
            PrimitiveTopologyType::Line);
    }

    bool operator==(const GridMaterialDesc&) const = default;
    size_t GetHash() const { return Core::HashOf(DebugSurfaceMaterialDesc::GetHash()); }
};