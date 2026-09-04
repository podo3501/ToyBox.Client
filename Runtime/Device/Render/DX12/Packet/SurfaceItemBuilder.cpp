#include "pch.h"
#include "SurfaceItemBuilder.h"
#include "RenderSortKey.h"
#include "Resource/Material/MaterialResource.h"
#include "Definition/RenderData.h"
#include "GameClient/Service/Render/Definition/View/SceneFrameData.h"

static RenderSurfaceItem BuildSurfaceItem(
    const DrawSurfaceItem& drawItem,
    std::optional<RasterPreset> rasterOverride)
{
    RenderSurfaceItem item{
        drawItem.mesh,
        drawItem.material,
        drawItem.shaderOverride,
        drawItem.world };

    auto material = static_cast<MaterialResource*>(drawItem.material.get());
    PipelineState pso = material->GetPipelineState(rasterOverride, drawItem.shaderOverride);
    item.sortKey = RenderSortKey::Build(pso.GetHash());
    item.pipelineState = pso;

    return item;
}

std::vector<RenderSurfaceItem> BuildSurfaceItems(
    std::vector<DrawSurfaceItem>& surfaces,
    std::optional<RasterPreset> rasterOverride)
{
    std::vector<RenderSurfaceItem> result;
    result.reserve(surfaces.size());

    for (auto& surface : surfaces)
        result.push_back(BuildSurfaceItem(surface, rasterOverride));

    std::sort(
        result.begin(),
        result.end(),
        [](auto& a, auto& b)
        {
            return a.sortKey < b.sortKey;
        });

    return result;
}