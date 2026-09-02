#include "pch.h"
#include "ClearGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"

ClearGraphBuilder::~ClearGraphBuilder() = default;
ClearGraphBuilder::ClearGraphBuilder(SwapChainPresenter& swapChain) noexcept :
    m_swapChain{ swapChain }
{}

void ClearGraphBuilder::Build(RenderGraph& graph, RGResourceID backBufferResID)
{
    auto& clear = graph.AddGraphicsPass("ClearBackBuffer");
    clear.Write(backBufferResID, RGAccess::RTV);
    clear.execute =
        [
            &swapChain = m_swapChain
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);
            swapChain.Clear(cmd, 0.13f, 0.13f, 0.16f, 1.0f);
        };
}