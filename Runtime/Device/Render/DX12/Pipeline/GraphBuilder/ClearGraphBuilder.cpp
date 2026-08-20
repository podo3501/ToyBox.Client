#include "pch.h"
#include "ClearGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"

ClearGraphBuilder::~ClearGraphBuilder() = default;
ClearGraphBuilder::ClearGraphBuilder(
    SwapChainPresenter& swapChain,
    RGResourceID backBufferResID) :
    m_swapChain{ swapChain },
    m_backBufferResID{ backBufferResID }
{}

void ClearGraphBuilder::Build(RenderGraph& graph)
{
    auto& clear = graph.AddGraphicsPass("ClearBackBuffer");
    clear.Write(m_backBufferResID, RGAccess::RTV);
    clear.gpuExecute =
        [
            &swapChain = m_swapChain
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);
            swapChain.Clear(cmd, 0.13f, 0.13f, 0.16f, 1.0f);
        };
}