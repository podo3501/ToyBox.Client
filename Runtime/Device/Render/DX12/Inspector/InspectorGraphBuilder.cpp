#include "pch.h"
#include "InspectorGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "Inspector/InspectorImageRenderer.h"

InspectorGraphBuilder::~InspectorGraphBuilder() = default;
InspectorGraphBuilder::InspectorGraphBuilder(
    InspectorImageRenderer& imageRenderer,
    SwapChainPresenter& swapChain) noexcept :
    m_imageRenderer{ imageRenderer },
    m_swapChain{ swapChain }
{}

void InspectorGraphBuilder::Build(
    RenderGraph& graph, 
    RGResourceID backBufferResID, 
    UINT srvIndex)
{
    auto& inspector = graph.AddGraphicsPass("Inspector");
    inspector.Write(backBufferResID, RGAccess::RTV);
    inspector.execute =
        [
            &imageInspector = m_imageRenderer,
            & swapChain = m_swapChain,
            srvIndex
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);
            swapChain.SetViewport(cmd);

            imageInspector.PrepareFrame();
            imageInspector.BeginFrame(cmd);

            imageInspector.BindPipeline(cmd);
            imageInspector.Draw(cmd, srvIndex);
        };
}