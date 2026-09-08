#pragma once
#include "ViewTargetPool.h"
#include "SceneViewGraphBuilder.h"
#include "OverlayViewGraphBuilder.h"
#include "CompositeGraphBuilder.h"

struct FramePacket;
struct ViewTargetPacket;
class Device;
class Renderers;
class DescriptorFactory;
class SwapChainPresenter;
class RenderGraph;
class RGResourceIDAllocator;

class ViewCompositionBuilder
{
public:
    ViewCompositionBuilder(
        Device& device,
        TaskScheduler& taskScheduler,
        DescriptorFactory& descFactory,
        Renderers& renderers,
        SwapChainPresenter& swapChain,
        RenderGraph& renderGraph,
        RGResourceIDAllocator& idAllocator);

    void Update();
    void ApplyResourceBindings(ResourceContext& resources);

    void Build(
        RGResourceID hBackBuffer,
        const FramePacket& framePacket,
        const FramePassContext& frameCtx);

private:
    ViewTargetResource& AcquireActiveViewTarget(
        const ViewTargetPacket& targetInfo,
        std::bitset<MaxViewCount>& activeViews);

    void AppendSceneViewOutputs(
        const FramePacket& framePacket, 
        const FramePassContext& frameCtx,
        std::bitset<MaxViewCount>& activeViews, 
        std::vector<ViewRenderOutput>& outputs);

    void AppendOverlayViewOutputs(
        const FramePacket& framePacket,
        std::bitset<MaxViewCount>& activeViews, 
        std::vector<ViewRenderOutput>& outputs);

    ViewTargetPool m_viewPool;
    SceneViewGraphBuilder m_sceneViewBuilder;
    OverlayViewGraphBuilder m_overlayViewBuilder;
    CompositeGraphBuilder m_compositeBuilder;

    RenderGraph& m_graph;
    RGResourceIDAllocator& m_idAllocator;
};