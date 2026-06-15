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

ForwardRenderPipeline::~ForwardRenderPipeline() = default;
ForwardRenderPipeline::ForwardRenderPipeline(Renderers* renderers, SwapChainPresenter* swapChain,
    DescriptorFactory* descFactory, ShadowResource* shadowRes) :
    m_swapChain(swapChain),
    m_shadowRes(shadowRes)
{
    BuildGraph(renderers, descFactory);
}

void ForwardRenderPipeline::BuildGraph(Renderers* renderers, DescriptorFactory* descFactory)
{
    m_hBackBuffer = m_graph.CreateRGHandle();
    m_hShadow = m_graph.CreateRGHandle();

    m_graph.ImportResource(m_hBackBuffer, RGAccess::Present);
    m_graph.ImportResource(m_hShadow, RGAccess::DepthWrite);

    ShadowGraphBuilder shadow(
        renderers->GetShadowRenderer(),
        descFactory,
        m_shadowRes,
        m_hShadow);

    OpaqueGraphBuilder opaque(
        renderers->GetSurfRenderer(),
        m_swapChain,
        m_shadowRes,
        m_hBackBuffer,
        m_hShadow);

    DebugSurfaceGraphBuilder debug(
        renderers->GetDebugSurfRenderer(),
        m_hBackBuffer);

    UIGraphBuilder ui(
        renderers->GetUIRenderer(),
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
}

void ForwardRenderPipeline::Render(CommandList& cmd, const DrawPacket& drawPacket, const FrameData& frame)
{
    TaskContext ctx;

    ctx.resources = std::make_shared<ResourceContext>();
    ctx.SetResource(m_hBackBuffer, m_swapChain->GetCurrentBackbuffer());
    ctx.SetResource(m_hShadow, m_shadowRes->GetResource());

    ctx.frame = frame;
    ctx.drawPacket = drawPacket;

    m_graph.Execute(cmd, m_compiledTasks, ctx);
}
