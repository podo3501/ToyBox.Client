#pragma once
#include "Renderer/Renderers.h"
#include "ViewTargetPool.h"
#include "ViewGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Graph/RGTypes.h"
#include "Resource/Internal/ShadowResource.h"
#include "Inspector/InspectorRenderers.h"
//Graph Builders
#include "GraphBuilder/ClearGraphBuilder.h"
#include "GraphBuilder/ViewTargetClearGraphBuilder.h"
#include "GraphBuilder/ShadowGraphBuilder.h"
#include "GraphBuilder/SkyboxGraphBuilder.h"
#include "GraphBuilder/OpaqueGraphBuilder.h"
#include "GraphBuilder/DebugSurfaceGraphBuilder.h"
#include "GraphBuilder/UIGraphBuilder.h"
#include "GraphBuilder/CompositeGraphBuilder.h"
#include "Inspector/InspectorGraphBuilder.h"
#include "TextSystem/Builder/FontAtlasUploadGraphBuilder.h"

struct FrameData;
struct FramePacket;
struct CompiledTask;
class Device;
class ShaderLibrary;
class ResourceFactory;
class DescriptorFactory;
class SwapChainPresenter;

class ForwardRenderPipeline
{
public:
    ~ForwardRenderPipeline();
    ForwardRenderPipeline(
        Device& device,
        SwapChainPresenter& swapChain,
        TaskScheduler& taskScheduler,
        DescriptorFactory& descFactory,
        ShaderLibrary& shaderLibrary,
        FontAtlasUploadGraphBuilder& fontUploadBuilder);
    bool Initialize(const Size& screenSize, const Size& shadowMapSize);
    void Update();
    void Render(
        CommandList& cmd, 
        FramePacket framePacket);
    void Resize(const Size& size);

private:
    std::vector<CompiledTask> BuildFrame(const FramePacket& framePacket);

    Device& m_device;
    SwapChainPresenter& m_swapChain;
    DescriptorFactory& m_descFactory;

    Renderers m_renderers;
    ViewTargetPool m_viewPool;
    ViewGraphBuilder m_viewBuilder;
    ShadowResource m_shadowRes; //이 클래스는 framereseource 클래스중의 하나. 프레임당 render가 필요한 리소스들.
    InspectorRenderers m_inspectorRenderers;

    RGResourceID m_hBackBuffer{ 0 };
    RGResourceID m_hShadow{ 0 };

    FontAtlasUploadGraphBuilder& m_fontUploadBuilder;
    ClearGraphBuilder m_clearBuilder;
    ShadowGraphBuilder m_shadowBuilder;
    CompositeGraphBuilder m_compositeBuilder;
    InspectorGraphBuilder m_inspectorBuilder;

    RenderGraph m_graph;

    std::optional<RGResourceID> m_debugTargetID;
};
