#pragma once
#include "Graph/RGTypes.h"

struct DirectionalLightData;
struct ViewPacket;
class RenderGraph;
class SurfaceRenderer;
class SwapChainPresenter;
class ShadowResource;

class OpaqueGraphBuilder
{
public:
    ~OpaqueGraphBuilder();
    OpaqueGraphBuilder() = delete;
    OpaqueGraphBuilder(
        SurfaceRenderer& surfRenderer, 
        SwapChainPresenter& swapChain,
        ShadowResource& shadowRes,
        RGResourceID backBufferResID, 
        RGResourceID shadowResID);

    void Build(
        RenderGraph& graph,
        const DirectionalLightData& light,
        std::shared_ptr<ViewPacket> packet,
        size_t viewIndex);
    
private:
    SurfaceRenderer& m_surfRenderer;
    SwapChainPresenter& m_swapChain;
    ShadowResource&m_shadowRes;
    RGResourceID m_backBufferResID;
    RGResourceID m_shadowResID;
};