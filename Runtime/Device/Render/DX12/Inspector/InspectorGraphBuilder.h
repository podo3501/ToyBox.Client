#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class InspectorImageRenderer;
class DescriptorFactory;
class SwapChainPresenter;

class InspectorGraphBuilder
{
public:
    ~InspectorGraphBuilder();
    InspectorGraphBuilder() = delete;
    InspectorGraphBuilder(
        InspectorImageRenderer& imageRenderer,
        DescriptorFactory& descFactory,
        SwapChainPresenter& swapChain) noexcept;

    void Build(
        RenderGraph& graph, 
        RGResourceID backBufferResID, 
        UINT srvIndex);

private:
    InspectorImageRenderer& m_imageRenderer;
    DescriptorFactory& m_descFactory;
    SwapChainPresenter& m_swapChain;

    RGResourceID m_backBufferResID{ InvalidRGID };
};