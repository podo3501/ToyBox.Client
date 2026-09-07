#pragma once

struct OverlayViewPacket;
class RenderGraph;
class UIRenderer;
class DescriptorFactory;
class ViewTargetResource;

class UIGraphBuilder
{
public:
    ~UIGraphBuilder();
    UIGraphBuilder() = delete;
    UIGraphBuilder(
        UIRenderer& uiRenderer, 
        DescriptorFactory& descFactory);

    void Build(
        RenderGraph& graph,
        std::shared_ptr<OverlayViewPacket> packet,
        const ViewTargetResource& target);

private:
    UIRenderer& m_uiRenderer;
    DescriptorFactory& m_descFactory;
};