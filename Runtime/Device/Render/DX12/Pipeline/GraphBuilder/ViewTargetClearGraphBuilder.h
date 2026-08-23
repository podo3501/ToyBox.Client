#pragma once

class RenderGraph;
class DescriptorFactory;
class ViewTargetResource;

class ViewTargetClearGraphBuilder
{
public:
    ~ViewTargetClearGraphBuilder();
    ViewTargetClearGraphBuilder(DescriptorFactory& descFactory) noexcept;

    void Build(RenderGraph& graph, const ViewTargetResource& target);

private:
    DescriptorFactory& m_descFactory;
};