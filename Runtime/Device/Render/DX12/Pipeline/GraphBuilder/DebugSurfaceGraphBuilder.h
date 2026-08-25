#pragma once

struct ViewPacket;
class RenderGraph;
class DebugSurfaceRenderer;
class DescriptorFactory;
class ViewTargetResource;

class DebugSurfaceGraphBuilder
{
public:
    ~DebugSurfaceGraphBuilder();
    DebugSurfaceGraphBuilder() = delete;
    DebugSurfaceGraphBuilder(
        DebugSurfaceRenderer& debugSurfRenderer, 
        DescriptorFactory& descFactory);

    void Build(
        RenderGraph& graph,
        std::shared_ptr<ViewPacket> packet,
        const ViewTargetResource& target);

private:
    DebugSurfaceRenderer& m_debugSurfRenderer;
    DescriptorFactory& m_descFactory;
};