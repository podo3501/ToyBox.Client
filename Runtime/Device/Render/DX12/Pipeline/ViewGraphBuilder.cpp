#include "pch.h"
#include "ViewGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Renderer/Renderers.h"
#include "Resource/Internal/ViewTargetResource.h"

ViewGraphBuilder::ViewGraphBuilder(
    Device& device,
    DescriptorFactory& descFactory,
    Renderers& renderers) :
    m_clearBuilder{ descFactory },
    m_skyboxBuilder{ renderers.GetSkyboxRenderer(), descFactory },
    m_opaqueBuilder{ renderers.GetSurfRenderer(), descFactory },
    m_debugBuilder{ renderers.GetDebugSurfRenderer(), descFactory },
    m_uiBuilder{ renderers.GetUIRenderer(), descFactory }
{}

RenderViewInfo ViewGraphBuilder::Build(
    RenderGraph& graph,
    const FramePassContext& frameCtx,
    const ViewTargetResource& target,
    const std::shared_ptr<ViewPacket>& view)
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
    if (!view->ui.empty())
        m_uiBuilder.Build(graph, view, target);

    graph.ExportResource(target.GetColorID(), RGAccess::SRV);
    graph.ExportResource(target.GetDepthID(), RGAccess::DepthWrite);

    return { view->viewport, target.GetHeapIndex(), target.GetColorID() };
}