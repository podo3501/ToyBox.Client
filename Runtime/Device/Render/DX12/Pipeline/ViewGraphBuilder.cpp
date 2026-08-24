#include "pch.h"
#include "ViewGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Renderer/Renderers.h"
#include "Resource/Internal/ViewTargetResource.h"

ViewGraphBuilder::ViewGraphBuilder(
    Device& device,
    TaskScheduler& taskScheduler,
    DescriptorFactory& descFactory,
    Renderers& renderers) :
    m_viewTargetPool{ device, taskScheduler, descFactory },
    m_clearBuilder{ descFactory },
    m_skyboxBuilder{ renderers.GetSkyboxRenderer(), descFactory },
    //m_opaqueBuilder{ renderers.GetSurfRenderer(), descFactory },
    m_debugBuilder{ renderers.GetDebugSurfRenderer(), descFactory },
    m_uiBuilder{ renderers.GetUIRenderer(), descFactory }
{
}

RenderViewInfo ViewGraphBuilder::Build(
    RenderGraph& graph,
    const FramePassContext& frameCtx,
    const std::shared_ptr<ViewPacket>& view)
{
    auto id = view->id;
    auto size = ToSize(
        static_cast<uint32_t>(view->viewport.width),
        static_cast<uint32_t>(view->viewport.height));
    ViewTargetResource& target = m_viewTargetPool.Acquire(id, size);

    graph.ImportResource(target.GetColorID(), RGAccess::RTV);
    graph.ImportResource(target.GetDepthID(), RGAccess::DepthWrite);

    m_clearBuilder.Build(graph, target);

    //if (view->environment)
    //    m_skyboxBuilder.Build(graph, view, target);
    //if (!view->surface.empty())
    //    m_opaqueBuilder.Build(graph, frameCtx.light, view, target, frameCtx.shadowRes, frameCtx.hShadow);
    //if (!view->debugSurface.empty())
    //    m_debugBuilder.Build(graph, view, target);
    //if (!view->ui.empty())
    //    m_uiBuilder.Build(graph, view, target);

    return { view->viewport, target.GetHeapIndex(), target.GetColorID() };
}

void ViewGraphBuilder::Update()
{
    m_viewTargetPool.Update();
}

void ViewGraphBuilder::PruneUnused(const std::unordered_set<uint32_t>& activeViews)
{
    m_viewTargetPool.PruneUnused(activeViews);
}