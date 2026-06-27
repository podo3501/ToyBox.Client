#include "pch.h"
#include "ForwardRenderPipeline.h"
#include "Graph/RenderGraph.h"
#include "Graph/TaskUtils.h"
#include "ShadowGraphBuilder.h"
#include "OpaqueGraphBuilder.h"
#include "DebugSurfaceGraphBuilder.h"
#include "UIGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Factory/DescriptorFactory.h"
#include "Command/CommandList.h"

ForwardRenderPipeline::~ForwardRenderPipeline() = default;
ForwardRenderPipeline::ForwardRenderPipeline(
    Device& device,
    SwapChainPresenter& swapChain,
    DescriptorFactory& descFactory,
    ShaderLibrary& shaderLibaray) :
    m_device{ device },
    m_swapChain{ swapChain },
    m_descFactory{ descFactory },
    m_renderers{ device, shaderLibaray }
{}

bool ForwardRenderPipeline::Initialize(const Size& screenSize, const Size& shadowMapSize)
{
    ReturnIfFalse(m_shadowRes.Initialize(m_device, m_descFactory, shadowMapSize));
    ReturnIfFalse(m_renderers.Initialize(screenSize));

    RenderGraph graph;

    m_hBackBuffer = RenderGraph::CreateRGResourceID();
    m_hShadow = RenderGraph::CreateRGResourceID();

    graph.ImportResource(m_hBackBuffer, RGAccess::Present);
    graph.ImportResource(m_hShadow, RGAccess::DepthWrite);

    ShadowGraphBuilder shadow(
        m_renderers.GetShadowRenderer(),
        m_descFactory,
        m_shadowRes,
        m_hShadow);

    OpaqueGraphBuilder opaque(
        m_renderers.GetSurfRenderer(),
        m_swapChain,
        m_shadowRes,
        m_hBackBuffer,
        m_hShadow);

    DebugSurfaceGraphBuilder debug(
        m_renderers.GetDebugSurfRenderer(),
        m_hBackBuffer);

    UIGraphBuilder ui(
        m_renderers.GetUIRenderer(),
        m_hBackBuffer);

    shadow.Build(graph);
    opaque.Build(graph);
    debug.Build(graph);
    ui.Build(graph);

    graph.ExportResource(m_hBackBuffer, RGAccess::Present);
    graph.ExportResource(m_hShadow, RGAccess::DepthWrite);

    m_compiledTasks = graph.Compile();

    return true;
}

void ForwardRenderPipeline::Render(CommandList& cmd, const DrawPacket& drawPacket, const FrameData& frame)
{
    TaskContext ctx;

    ctx.resources = std::make_shared<ResourceContext>();
    ctx.SetResource(m_hBackBuffer, m_swapChain.GetCurrentBackbuffer());
    ctx.SetResource(m_hShadow, m_shadowRes.GetResource());

    ctx.frame = frame;
    ctx.drawPacket = drawPacket;

    auto& bindlessAllocator = m_descFactory.GetBindlessAllocator();
    cmd.SetBindlessHeap(bindlessAllocator.GetHeap());

    ExecuteRenderPipeline(cmd, m_compiledTasks, ctx);
}

void ForwardRenderPipeline::Resize(const Size& size)
{
    m_renderers.SetScreenSize(size);
}

