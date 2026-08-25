#pragma once
#include "GraphBuilder/ViewTargetClearGraphBuilder.h"
#include "GraphBuilder/SkyboxGraphBuilder.h"
#include "GraphBuilder/OpaqueGraphBuilder.h"
#include "GraphBuilder/DebugSurfaceGraphBuilder.h"
#include "GraphBuilder/UIGraphBuilder.h"

class Device;
class TaskScheduler;
class DescriptorFactory;
class Renderers;
class RenderGraph;
class ShadowResource;
struct RenderView;
struct RenderViewInfo;

// 프레임 전체에 걸쳐 고정되는 컨텍스트 (뷰 루프 시작 전 한 번만 구성)
struct FramePassContext
{
    const DirectionalLightData& light;
    const ShadowResource& shadowRes;
    RGResourceID hShadow;
};

class ViewGraphBuilder
{
public:
    ViewGraphBuilder(
        Device& device,
        DescriptorFactory& descFactory,
        Renderers& renderers);

    RenderViewInfo Build(
        RenderGraph& graph,
        const FramePassContext& frameCtx,
        const ViewTargetResource& target,
        const std::shared_ptr<ViewPacket>& view);

private:
    ViewTargetClearGraphBuilder m_clearBuilder;
    SkyboxGraphBuilder m_skyboxBuilder;
    OpaqueGraphBuilder m_opaqueBuilder;
    DebugSurfaceGraphBuilder m_debugBuilder;
    UIGraphBuilder m_uiBuilder;
};