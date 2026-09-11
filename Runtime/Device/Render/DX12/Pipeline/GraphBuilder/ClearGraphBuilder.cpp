#include "pch.h"
#include "ClearGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "Task/Types/TaskCommandLists.h"

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
        (TaskCommandLists cmds, TaskContext& ctx)
        {
            CommandList& cmd = cmds.Single();

            swapChain.SetRenderTarget(cmd);
            swapChain.Clear(cmd, 0.13f, 0.13f, 0.16f, 1.0f);
        };
}