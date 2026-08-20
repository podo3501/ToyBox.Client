#include "pch.h"
#include "UIGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "../Renderer/UIRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Brush/BrushResource.h"

UIGraphBuilder::~UIGraphBuilder() = default;
UIGraphBuilder::UIGraphBuilder(
    UIRenderer& uiRenderer, 
    SwapChainPresenter& swapChain,
    RGResourceID backBufferResID) :
    m_uiRenderer{ uiRenderer },
    m_swapChain{ swapChain },
    m_backBufferResID{ backBufferResID }
{}

void UIGraphBuilder::Build(RenderGraph& graph)
{
    auto& ui = graph.AddGraphicsPass("UI");
    ui.Write(m_backBufferResID, RGAccess::RTV);
    ui.gpuExecute =
        [
            &swapChain = m_swapChain,
            &uiRenderer = m_uiRenderer
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);
            swapChain.SetViewport(cmd, ctx.packet->viewport);
            uiRenderer.BeginFrame(cmd);

            for (auto& uiItem : ctx.packet->ui)
            {
                auto mesh = static_cast<MeshResource*>(uiItem.mesh.get());
                auto brush = static_cast<BrushResource*>(uiItem.brush.get());

                uiRenderer.Draw(cmd, *mesh, *brush, uiItem.world, uiItem.source);
            }
        };
}