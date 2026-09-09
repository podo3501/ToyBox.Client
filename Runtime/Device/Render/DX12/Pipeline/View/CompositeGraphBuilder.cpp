#include "pch.h"
#include "CompositeGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "Resource/Internal/ViewTargetResource.h"
#include "Pipeline/Renderer/CompositeRenderer.h"

CompositeGraphBuilder::CompositeGraphBuilder(
    CompositeRenderer& compositeRenderer,
    SwapChainPresenter& swapChain) noexcept :
    m_compositeRenderer{ compositeRenderer },
    m_swapChain{ swapChain }
{}

void CompositeGraphBuilder::Build(
    RenderGraph& graph,
    RGResourceID backBufferResID,
    const std::vector<ViewRenderOutput>& viewOutputs)
{
    auto& composite = graph.AddGraphicsPass("Composite");

    for (auto& info : viewOutputs)
        composite.Read(info.colorID, RGAccess::SRV);
    composite.Write(backBufferResID, RGAccess::RTV);

    composite.execute =
        [
            &compositeRenderer = m_compositeRenderer,
            &swapChain = m_swapChain,
            viewOutputs
        ]
        (std::span<CommandList*> cmds, TaskContext& ctx)
        {
            CommandList& cmd = GetSingleCommandList(cmds);
            swapChain.SetRenderTarget(cmd);

            compositeRenderer.PrepareDraw(cmd);
            for (auto& info : viewOutputs)
            {
                swapChain.SetViewport(cmd, info.viewport);
                compositeRenderer.Draw(cmd, info.heapIndex);
            }
        };
}