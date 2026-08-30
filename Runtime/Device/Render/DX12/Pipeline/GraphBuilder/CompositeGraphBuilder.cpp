#include "pch.h"
#include "CompositeGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "Resource/Internal/ViewTargetResource.h"
#include "../Renderer/CompositeRenderer.h"

CompositeGraphBuilder::CompositeGraphBuilder(
    CompositeRenderer& compositeRenderer,
    SwapChainPresenter& swapChain) noexcept :
    m_compositeRenderer{ compositeRenderer },
    m_swapChain{ swapChain }
{}

void CompositeGraphBuilder::Build(
    RenderGraph& graph,
    RGResourceID backBufferResID,
    const std::vector<RenderViewInfo>& renderViewInfos)
{
    auto& composite = graph.AddGraphicsPass("Composite");

    for (auto& info : renderViewInfos)
        composite.Read(info.colorID, RGAccess::SRV);
    composite.Write(backBufferResID, RGAccess::RTV);

    composite.gpuExecute =
        [
            &swapChain = m_swapChain,
            &compositeRenderer = m_compositeRenderer,
            renderViewInfos
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);

            compositeRenderer.PrepareDraw(cmd);
            for (auto& info : renderViewInfos)
            {
                swapChain.SetViewport(cmd, info.viewport);
                compositeRenderer.Draw(cmd, info.heapIndex);
            }
        };
}