#include "pch.h"
#include "ViewCompositionBuilder.h"
#include "Graph/RenderGraph.h"
#include "Pipeline/Renderers.h"

ViewCompositionBuilder::ViewCompositionBuilder(
    Device& device,
    TaskScheduler& taskScheduler,
    DescriptorFactory& descFactory,
    Renderers& renderers,
    SwapChainPresenter& swapChain,
    RenderGraph& renderGraph,
    RGResourceIDAllocator& idAllocator) :
    m_viewPool{ device, taskScheduler, descFactory },
    m_sceneViewBuilder{ device, descFactory, renderers },
    m_overlayViewBuilder{ descFactory, renderers },
    m_compositeBuilder{ renderers.GetCompositeRenderer(), swapChain },
    m_graph{ renderGraph },
    m_idAllocator{ idAllocator }
{}

void ViewCompositionBuilder::Build(
    RGResourceID hBackBuffer,
    const FramePacket& framePacket,
    const FramePassContext& frameCtx)
{
    std::vector<ViewRenderOutput> viewOutputs;
    viewOutputs.reserve(framePacket.sceneViews.size() + framePacket.overlayViews.size());
    std::bitset<MaxViewCount> activeViews;

    AppendSceneViewOutputs(framePacket, frameCtx, activeViews, viewOutputs);
    AppendOverlayViewOutputs(framePacket, activeViews, viewOutputs);

    m_viewPool.PruneUnused(activeViews);
    std::stable_sort(viewOutputs.begin(), viewOutputs.end(),
        [](const ViewRenderOutput& lhs, const ViewRenderOutput& rhs) { return lhs.id < rhs.id; });

    m_compositeBuilder.Build(m_graph, hBackBuffer, viewOutputs);
}

void ViewCompositionBuilder::Update()
{
    m_viewPool.Update();
}

void ViewCompositionBuilder::ApplyResourceBindings(ResourceContext& resources)
{
    m_viewPool.ApplyResourceBindings(resources);
}

ViewTargetResource& ViewCompositionBuilder::AcquireActiveViewTarget(
    const ViewTargetPacket& targetInfo,
    std::bitset<MaxViewCount>& activeViews)
{
    activeViews.set(targetInfo.id);
    auto size = ToSize(targetInfo.viewport.width, targetInfo.viewport.height);
    return m_viewPool.Acquire(targetInfo.id, m_idAllocator, size);
}

void ViewCompositionBuilder::AppendSceneViewOutputs(
    const FramePacket& framePacket,
    const FramePassContext& frameCtx,
    std::bitset<MaxViewCount>& activeViews,
    std::vector<ViewRenderOutput>& outputs)
{
    for (auto& view : framePacket.sceneViews)
    {
        ViewTargetResource& target = AcquireActiveViewTarget(view->target, activeViews);
        outputs.push_back(m_sceneViewBuilder.Build(m_graph, frameCtx, target, view));
    }
}

void ViewCompositionBuilder::AppendOverlayViewOutputs(
    const FramePacket& framePacket,
    std::bitset<MaxViewCount>& activeViews,
    std::vector<ViewRenderOutput>& outputs)
{
    for (auto& view : framePacket.overlayViews)
    {
        ViewTargetResource& target = AcquireActiveViewTarget(view->target, activeViews);
        outputs.push_back(m_overlayViewBuilder.Build(m_graph, target, view));
    }
}
