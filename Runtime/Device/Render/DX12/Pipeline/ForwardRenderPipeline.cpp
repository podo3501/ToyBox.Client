#include "pch.h"
#include "ForwardRenderPipeline.h"
#include "Renderer/Renderers.h"
#include "Resource/ShadowResource.h"
#include "ShadowGraphBuilder.h"
#include "OpaqueGraphBuilder.h"
#include "DebugSurfaceGraphBuilder.h"
#include "UIGraphBuilder.h"
#include "FrameEndGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/DescriptorAllocator.h"
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
    m_renderers.Initialize(screenSize);

    m_hBackBuffer = m_graph.CreateRGHandle();
    m_hShadow = m_graph.CreateRGHandle();

    m_graph.ImportResource(m_hBackBuffer, RGAccess::Present);
    m_graph.ImportResource(m_hShadow, RGAccess::DepthWrite);

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

    FrameEndGraphBuilder end(
        m_hBackBuffer,
        m_hShadow);

    shadow.Build(m_graph);
    opaque.Build(m_graph);
    debug.Build(m_graph);
    ui.Build(m_graph);
    end.Build(m_graph);

    m_compiledTasks = m_graph.Compile();

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

    m_graph.Execute(cmd, m_compiledTasks, ctx);
}

void ForwardRenderPipeline::Resize(const Size& size)
{
    m_renderers.SetScreenSize(size);
}

