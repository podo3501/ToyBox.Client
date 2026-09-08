#pragma once
#include "GraphBuilder/ViewTargetClearGraphBuilder.h"
#include "GraphBuilder/SkyboxGraphBuilder.h"
#include "GraphBuilder/OpaqueGraphBuilder.h"
#include "GraphBuilder/DebugSurfaceGraphBuilder.h"

struct ViewRenderOutput;
class Device;
class DescriptorFactory;
class Renderers;

// 프레임 전체에 걸쳐 고정되는 컨텍스트 (뷰 루프 시작 전 한 번만 구성)
struct FramePassContext 
{
    const DirectionalLightData& light;
    const ShadowResource& shadowRes;
    RGResourceID hShadow;
};

class SceneViewGraphBuilder
{
public:
    SceneViewGraphBuilder(
        Device& device, 
        DescriptorFactory& descFactory, 
        Renderers& renderers);

    ViewRenderOutput Build(
        RenderGraph& graph,
        const FramePassContext& frameCtx,
        const ViewTargetResource& target,
        const std::shared_ptr<SceneViewPacket>& view);

private:
    ViewTargetClearGraphBuilder m_clearBuilder;
    SkyboxGraphBuilder m_skyboxBuilder;
    OpaqueGraphBuilder m_opaqueBuilder;
    DebugSurfaceGraphBuilder m_debugBuilder;
};