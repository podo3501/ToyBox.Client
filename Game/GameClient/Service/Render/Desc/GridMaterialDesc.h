#pragma once
#include "SurfaceMaterialDesc.h"

struct GridMaterialDesc : public SurfaceMaterialDesc
{
    GridMaterialDesc()
    {
        surfType = SurfaceType::Grid;
        pipelineState = PipelineLibrary::Get(
            ShadingModel::Grid, 
            RasterPreset::Default, 
            PrimitiveTopologyType::Line);
    }

    bool operator==(const GridMaterialDesc&) const = default;
    size_t GetHash() const { return Core::HashOf(SurfaceMaterialDesc::GetHash()); }
};