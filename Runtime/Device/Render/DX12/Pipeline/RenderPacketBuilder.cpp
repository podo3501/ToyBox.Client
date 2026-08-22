#include "pch.h"
#include "RenderPacketBuilder.h"
#include "RenderSortKey.h"
#include "Resource/Material/MaterialResource.h"
#include "Resource/Material/DebugMaterialResource.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "Core/RenderData.h"
#include "TextSystem/TextSystem.h"
#include "GameClient/Service/Render/Definition/View/SceneViewData.h"

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

static RenderTextItem ToRenderTextItem(DrawTextItem& text)
{
    Assert(text.font);
    Rect normalized = text.bounds;
    normalized.Normalize(); // 뒤집힌 rect 방어

    RenderTextItem item;
    item.fontRes = text.font;
    item.mode = text.mode;
    item.fontSize = text.size;
    item.position = Core::Vector2{ normalized.Left(), normalized.Top() };
    item.size = Core::Vector2{ normalized.width, normalized.height };
    item.layout = text.layout;
    item.runs = std::move(text.runs);

    return item;
}

std::shared_ptr<ViewPacket> BuildViewPacket(SceneViewData&& view, TextSystem& textSystem)
{
    auto packet = std::make_shared<ViewPacket>();

    //ViewContext
    packet->camera = view.context.camera;
    packet->uiCamera = view.context.uiCamera;
    packet->viewport = view.context.viewport;
    auto rasterOverride = view.context.renderOverride.rasterPreset;

    //ViewDrawList
    if (view.draws.environment)
        packet->environment = std::static_pointer_cast<EnvironmentResource>(view.draws.environment);

    packet->surface.reserve(view.draws.surfaces.size());
    for (auto& surface : view.draws.surfaces)
        packet->surface.push_back(BuildSurfaceItem(surface, rasterOverride));

    packet->debugSurface.reserve(view.draws.debugSurfaces.size());
    for (auto& debugSurface : view.draws.debugSurfaces)
        packet->debugSurface.push_back(BuildDebugSurfaceItem(debugSurface));

    packet->ui.reserve(view.draws.ui.size());
    for (auto& ui : view.draws.ui)
        packet->ui.push_back(RenderUIItem{ ui.mesh, ui.brush, ui.world, ui.source });

    if (!view.draws.texts.empty())
    {
        std::vector<RenderTextItem> textItems;
        textItems.reserve(view.draws.texts.size());
        for (auto& text : view.draws.texts)
        {
            if (text.runs.empty()) continue;
            textItems.push_back(ToRenderTextItem(text));
        }
        auto textUIItems = textSystem.BuildDrawItems(textItems);

        packet->ui.insert(packet->ui.end(),
            std::make_move_iterator(textUIItems.begin()),
            std::make_move_iterator(textUIItems.end()));
    }

    std::sort(packet->surface.begin(), packet->surface.end(),
        [](auto& a, auto& b) { return a.sortKey < b.sortKey; });
    std::sort(packet->debugSurface.begin(), packet->debugSurface.end(),
        [](auto& a, auto& b) { return a.sortKey < b.sortKey; });
    std::sort(packet->ui.begin(), packet->ui.end(),
        [](auto& a, auto& b) { return a.sortKey < b.sortKey; });

    return packet;
}