#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class InspectorImageRenderer;
class SwapChainPresenter;

class InspectorGraphBuilder
{
public:
    ~InspectorGraphBuilder();
    InspectorGraphBuilder() = delete;
    explicit InspectorGraphBuilder(
        InspectorImageRenderer& imageRenderer,
        SwapChainPresenter& swapChain) noexcept;

    void Build(
        RenderGraph& graph, 
        RGResourceID backBufferResID, 
        UINT srvIndex);

private:
    InspectorImageRenderer& m_imageRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};