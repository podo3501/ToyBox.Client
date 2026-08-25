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
        DescriptorFactory& descFactory);

    void Build(
        RenderGraph& graph,
        const DirectionalLightData& light,
        const ShadowResource& shadowRes,
        RGResourceID shadowResID,
        std::shared_ptr<ViewPacket> packet,
        const ViewTargetResource& target);
    
private:
    SurfaceRenderer& m_surfRenderer;
    DescriptorFactory& m_descFactory;
};