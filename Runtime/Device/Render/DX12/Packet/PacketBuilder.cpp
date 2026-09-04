#include "pch.h"
#include "PacketBuilder.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "Definition/RenderData.h"
#include "SurfaceItemBuilder.h"
#include "DebugSurfaceItemBuilder.h"
#include "UIItemBuilder.h"
#include "GameClient/Service/Render/Definition/View/SceneFrameData.h"

static Rect ResolveViewport(const std::optional<Rect>& requestedViewport, const Size& screenSize)
{
    return requestedViewport.value_or(
        Rect{ 0.f, 0.f, static_cast<float>(screenSize.width), static_cast<float>(screenSize.height) });
}

static std::shared_ptr<ViewPacket> BuildViewPacket(
    SceneViewData&& view,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider,
    const Size& screenSize)
{
    auto packet = std::make_shared<ViewPacket>();

    //ViewContext
    packet->id = view.context.id;
    packet->camera = view.context.camera;
    packet->uiProj = view.context.uiProj;
    packet->viewport = ResolveViewport(view.context.viewport, screenSize);
    packet->localViewport = Rect{ 0.f, 0.f, packet->viewport.width, packet->viewport.height };

    if (view.draws.environment)
        packet->environment = std::static_pointer_cast<EnvironmentResource>(view.draws.environment);

    packet->surface = BuildSurfaceItems(view.draws.surfaces, view.context.renderOverride.rasterPreset);
    packet->debugSurface = BuildDebugSurfaceItems(view.draws.debugSurfaces);
    packet->ui = BuildUIItems(view.draws, textSystem, meshProvider);

    return packet;
}

static std::vector<std::shared_ptr<ViewPacket>> BuildViews(
    std::vector<SceneViewData>& views,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider,
    const Size& screenSize)
{
    std::vector<std::shared_ptr<ViewPacket>> result;
    result.reserve(std::max<size_t>(1, views.size()));

    for (auto& view : views)
    {
        result.push_back(
            BuildViewPacket(
                std::move(view),
                textSystem,
                meshProvider,
                screenSize));
    }

    return result;
}

static std::vector<RenderShadowCasterItem> BuildShadowCasters(
    std::vector<DrawShadowCasterItem>& casters)
{
    std::vector<RenderShadowCasterItem> result;
    result.reserve(casters.size());

    for (auto& caster : casters)
    {
        result.push_back(
            RenderShadowCasterItem{
                std::move(caster.mesh),
                caster.world
            });
    }

    return result;
}

FramePacket BuildPacket(
    SceneFrameData& frame,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider,
    const Size& screenSize)
{
    FramePacket packet;
    packet.light = std::move(frame.light);
    packet.views = BuildViews(frame.views, textSystem, meshProvider, screenSize);
    packet.shadowCasters = BuildShadowCasters(frame.shadowCasters);

    return packet;
}
