#pragma once

struct ViewPacket;
class RenderGraph;
class SkyboxRenderer;
class DescriptorFactory;
class ViewTargetResource;

class SkyboxGraphBuilder
{
public:
    ~SkyboxGraphBuilder();
    SkyboxGraphBuilder() = delete;
    SkyboxGraphBuilder(
        SkyboxRenderer& skyboxRenderer,
        DescriptorFactory& descFactory);

    void Build(
        RenderGraph& graph,
        std::shared_ptr<ViewPacket> packet,
        const ViewTargetResource& target);

private:
    SkyboxRenderer& m_skyboxRenderer;
    DescriptorFactory& m_descFactory;
};
