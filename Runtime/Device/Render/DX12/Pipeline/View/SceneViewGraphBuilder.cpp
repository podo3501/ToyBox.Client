#include "pch.h"
#include "SceneViewGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Pipeline/Renderers.h"
#include "Resource/Internal/ViewTargetResource.h"
#include "Definition/RenderData.h"

SceneViewGraphBuilder::SceneViewGraphBuilder(
    Device& device,
    DescriptorFactory& descFactory,
    Renderers& renderers) :
    m_clearBuilder{ descFactory },
    m_skyboxBuilder{ renderers.GetSkyboxRenderer(), descFactory },
    m_opaqueBuilder{ renderers.GetSurfRenderer(), descFactory },
    m_debugBuilder{ renderers.GetDebugSurfRenderer(), descFactory }
{}

ViewRenderOutput SceneViewGraphBuilder::Build(
    RenderGraph& graph,
    const FramePassContext& frameCtx,
    const ViewTargetResource& target,
    const std::shared_ptr<SceneViewPacket>& view)
{
    graph.ImportResource(target.GetColorID(), RGAccess::SRV);
    graph.ImportResource(target.GetDepthID(), RGAccess::DepthWrite);

    m_clearBuilder.Build(graph, target);

    if (view->environment)
        m_skyboxBuilder.Build(graph, view, target);
    if (!view->surface.empty())
        m_opaqueBuilder.Build(graph, frameCtx.light, frameCtx.shadowRes, frameCtx.hShadow, view, target);
    if (!view->debugSurface.empty())
        m_debugBuilder.Build(graph, view, target);

    graph.ExportResource(target.GetColorID(), RGAccess::SRV);
    graph.ExportResource(target.GetDepthID(), RGAccess::DepthWrite);

    return { view->target.id, view->target.viewport, target.GetHeapIndex(), target.GetColorID() };
}