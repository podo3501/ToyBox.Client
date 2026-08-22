#pragma once
#include "Graph/RGTypes.h"

struct ViewPacket;
class RenderGraph;
class SkyboxRenderer;
class SwapChainPresenter;

class SkyboxGraphBuilder
{
public:
    ~SkyboxGraphBuilder();
    SkyboxGraphBuilder() = delete;
    SkyboxGraphBuilder(
        SkyboxRenderer& skyboxRenderer,
        SwapChainPresenter& swapChain,
        RGResourceID backBufferResID);

    void Build(
        RenderGraph& graph,
        std::shared_ptr<ViewPacket> packet,
        size_t viewIndex);

private:
    SkyboxRenderer& m_skyboxRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};
