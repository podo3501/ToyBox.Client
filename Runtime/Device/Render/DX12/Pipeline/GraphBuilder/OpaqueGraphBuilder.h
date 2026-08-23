#pragma once
#include "Graph/RGTypes.h"

struct DirectionalLightData;
struct ViewPacket;
class RenderGraph;
class SurfaceRenderer;
class DescriptorFactory;
class ShadowResource;
class ViewTargetResource;

class OpaqueGraphBuilder
{
public:
    ~OpaqueGraphBuilder();
    OpaqueGraphBuilder() = delete;
    OpaqueGraphBuilder(
        SurfaceRenderer& surfRenderer, 
        DescriptorFactory& descFactory,
        ShadowResource& shadowRes,
        RGResourceID shadowResID);

    void Build(
        RenderGraph& graph,
        const DirectionalLightData& light,
        std::shared_ptr<ViewPacket> packet,
        size_t viewIndex,
        const ViewTargetResource& target);
    
private:
    SurfaceRenderer& m_surfRenderer;
    DescriptorFactory& m_descFactory;
    ShadowResource&m_shadowRes;
    RGResourceID m_shadowResID;
};