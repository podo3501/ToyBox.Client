#pragma once
#include "Resource/ShadowResource.h"
#include "Renderer/Renderers.h"
#include "Graph/RenderGraph.h"
#include "Graph/RGTypes.h"
#include "Inspector/InspectorRenderers.h"
//Graph Builders
#include "GraphBuilder/ClearGraphBuilder.h"
#include "GraphBuilder/ShadowGraphBuilder.h"
#include "GraphBuilder/SkyboxGraphBuilder.h"
#include "GraphBuilder/OpaqueGraphBuilder.h"
#include "GraphBuilder/DebugSurfaceGraphBuilder.h"
#include "GraphBuilder/UIGraphBuilder.h"
#include "Inspector/InspectorGraphBuilder.h"
#include "TextSystem/Builder/FontAtlasUploadGraphBuilder.h"

struct FrameData;
struct RenderPacket;
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
        DescriptorFactory& descFactory,
        ShaderLibrary& shaderLibrary,
        FontAtlasUploadGraphBuilder& fontUploadBuilder);
    bool Initialize(const Size& screenSize, const Size& shadowMapSize);
    void Render(
        CommandList& cmd, 
        std::vector<std::shared_ptr<RenderPacket>> packets,
        const FrameData& frame);
    void Resize(const Size& size);

private:
    std::vector<CompiledTask> BuildFrame(const RenderPacket* packet);

    Device& m_device;
    SwapChainPresenter& m_swapChain;
    DescriptorFactory& m_descFactory;

    ShadowResource m_shadowRes; //이 클래스는 framereseource 클래스중의 하나. 프레임당 render가 필요한 리소스들.
    Renderers m_renderers;
    InspectorRenderers m_inspectorRenderers;

    RGResourceID m_hBackBuffer{ 0 };
    RGResourceID m_hShadow{ 0 };

    FontAtlasUploadGraphBuilder& m_fontUploadBuilder;
    ClearGraphBuilder m_clearBuilder;
    ShadowGraphBuilder m_shadowBuilder;
    SkyboxGraphBuilder m_skyboxBuilder;
    OpaqueGraphBuilder m_opaqueBuilder;
    DebugSurfaceGraphBuilder m_debugBuilder;
    UIGraphBuilder m_uiBuilder;
    InspectorGraphBuilder m_inspectorBuilder;

    RenderGraph m_graph;
};
