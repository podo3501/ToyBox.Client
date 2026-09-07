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

static void FillTarget(
    ViewTargetPacket& target,
    const ViewTargetInfo& targetInfo,
    const Size& screenSize)
{
    target.id = targetInfo.id;
    target.camera = targetInfo.camera;
    target.viewport = ResolveViewport(targetInfo.viewport, screenSize);
    target.localViewport = Rect{ 0.f, 0.f, target.viewport.width, target.viewport.height };
}

static std::shared_ptr<SceneViewPacket> BuildSceneViewPacket(
    SceneViewData&& view,
    const Size& screenSize)
{
    auto packet = std::make_shared<SceneViewPacket>();
    FillTarget(packet->target, view.context.target, screenSize);

    if (view.draws.environment)
        packet->environment = std::static_pointer_cast<EnvironmentResource>(view.draws.environment);
    packet->surface = BuildSurfaceItems(view.draws.surfaces, view.context.renderOverride.rasterPreset);
    packet->debugSurface = BuildDebugSurfaceItems(view.draws.debugSurfaces);

    return packet;
}

static std::shared_ptr<OverlayViewPacket> BuildOverlayViewPacket(
    OverlayViewData&& view,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider,
    const Size& screenSize)
{
    auto packet = std::make_shared<OverlayViewPacket>();
    FillTarget(packet->target, view.context.target, screenSize);

    packet->ui = BuildUIItems(view.draws, textSystem, meshProvider);
    return packet;
}

static std::vector<std::shared_ptr<SceneViewPacket>> BuildSceneViews(
    std::vector<SceneViewData>& views,
    const Size& screenSize)
{
    std::vector<std::shared_ptr<SceneViewPacket>> result;
    result.reserve(views.size());

    for (auto& view : views)
        result.push_back(BuildSceneViewPacket(std::move(view), screenSize));

    return result;
}

static std::vector<std::shared_ptr<OverlayViewPacket>> BuildOverlayViews(
    std::vector<OverlayViewData>& views,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider,
    const Size& screenSize)
{
    std::vector<std::shared_ptr<OverlayViewPacket>> result;
    result.reserve(views.size());

    for (auto& view : views)
        result.push_back(BuildOverlayViewPacket(std::move(view), textSystem, meshProvider, screenSize));

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
    packet.shadowCasters = BuildShadowCasters(frame.shadowCasters);
    packet.sceneViews = BuildSceneViews(frame.sceneViews, screenSize);
    packet.overlayViews = BuildOverlayViews(frame.overlayViews, textSystem, meshProvider, screenSize);

    return packet;
}
