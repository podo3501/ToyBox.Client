#pragma once
#include "Renderers.h"
#include "Graph/RenderGraph.h"
#include "Graph/RGResourceIDAllocator.h"
#include "Resource/Internal/ShadowResource.h"
#include "Inspector/InspectorRenderers.h"
//Graph Builders
#include "View/ViewCompositionBuilder.h"
#include "GraphBuilder/ClearGraphBuilder.h"
#include "GraphBuilder/ShadowGraphBuilder.h"
#include "Inspector/InspectorGraphBuilder.h"
#include "TextSystem/Builder/FontAtlasUploadGraphBuilder.h"

struct FrameData;
struct FramePacket;
struct CompiledTask;
class Device;
class CommandScheduler;
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
    bool Initialize(const Size& screenSize);
    void Update();
    CommandList* Render(
        CommandList* cmd, 
        CommandScheduler& cmdScheduler,
        FramePacket framePacket);
    void Resize(const Size& size);

private:
    std::vector<CompiledTask> BuildFrame(const FramePacket& framePacket);

    Device& m_device;
    SwapChainPresenter& m_swapChain;
    DescriptorFactory& m_descFactory;

    RenderGraph m_graph;
    RGResourceIDAllocator m_idAllocator;

    Renderers m_renderers;

    ShadowResource m_shadowRes; //이 클래스는 framereseource 클래스중의 하나. 프레임당 render가 필요한 리소스들.
    InspectorRenderers m_inspectorRenderers;

    RGResourceID m_hBackBuffer{ InvalidRGID };
    RGResourceID m_hShadow{ InvalidRGID };

    FontAtlasUploadGraphBuilder& m_fontUploadBuilder;
    ClearGraphBuilder m_clearBuilder;
    ShadowGraphBuilder m_shadowBuilder;
    ViewCompositionBuilder m_viewComposition;
    InspectorGraphBuilder m_inspectorBuilder;
};
