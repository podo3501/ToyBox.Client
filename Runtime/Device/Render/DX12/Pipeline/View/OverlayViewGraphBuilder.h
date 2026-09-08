#pragma once
#include "GraphBuilder/ViewTargetClearGraphBuilder.h"
#include "GraphBuilder/UIGraphBuilder.h"

struct ViewRenderOutput;
class RenderGraph;
class DescriptorFactory;
class Renderers;

class OverlayViewGraphBuilder
{
public:
    OverlayViewGraphBuilder(
        DescriptorFactory& descFactory, 
        Renderers& renderers);

    ViewRenderOutput Build(
        RenderGraph& graph,
        const ViewTargetResource& target,
        const std::shared_ptr<OverlayViewPacket>& view);

private:
    ViewTargetClearGraphBuilder m_clearBuilder;
    UIGraphBuilder m_uiBuilder;
};