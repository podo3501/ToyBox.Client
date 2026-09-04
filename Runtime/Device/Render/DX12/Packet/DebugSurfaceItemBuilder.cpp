#include "pch.h"
#include "DebugSurfaceItemBuilder.h"
#include "RenderSortKey.h"
#include "Resource/Material/DebugMaterialResource.h"
#include "Definition/RenderData.h"
#include "GameClient/Service/Render/Definition/View/SceneFrameData.h"

static RenderDebugSurfaceItem BuildDebugSurfaceItem(const DrawDebugSurfaceItem& drawItem)
{
    RenderDebugSurfaceItem item{
        drawItem.mesh,
        drawItem.material,
        drawItem.world };

    auto debugMaterial = static_cast<DebugMaterialResource*>(drawItem.material.get());
    item.sortKey = RenderSortKey::Build(debugMaterial->GetPipelineState().GetHash());

    return item;
}

std::vector<RenderDebugSurfaceItem> BuildDebugSurfaceItems(
    std::vector<DrawDebugSurfaceItem>& debugSurfaces)
{
    std::vector<RenderDebugSurfaceItem> result;
    result.reserve(debugSurfaces.size());

    for (auto& debugSurface : debugSurfaces)
        result.push_back(BuildDebugSurfaceItem(debugSurface));

    std::sort(
        result.begin(),
        result.end(),
        [](auto& a, auto& b)
        {
            return a.sortKey < b.sortKey;
        });

    return result;
}