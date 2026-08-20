#include "pch.h"
#include "DebugSurfaceGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "../Renderer/DebugSurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/DebugMaterialResource.h"

DebugSurfaceGraphBuilder::~DebugSurfaceGraphBuilder() = default;
DebugSurfaceGraphBuilder::DebugSurfaceGraphBuilder(
    DebugSurfaceRenderer& debugSurfRenderer, 
    SwapChainPresenter& swapChain,
    RGResourceID backBufferResID) :
    m_debugSurfRenderer{ debugSurfRenderer },
    m_swapChain{ swapChain },
    m_backBufferResID{ backBufferResID }
{}

void DebugSurfaceGraphBuilder::Build(RenderGraph& graph)
{
    auto& grid = graph.AddGraphicsPass("DebugSurface");
    grid.Write(m_backBufferResID, RGAccess::RTV);
    grid.gpuExecute =
        [
            &swapChain = m_swapChain,
            &debugSurfRenderer = m_debugSurfRenderer
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);
            swapChain.SetViewport(cmd, ctx.packet->viewport);

            debugSurfRenderer.PrepareFrame(ctx.frame.camera);
            debugSurfRenderer.BeginFrame(cmd);

            for (auto& item : ctx.packet->debugSurface)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                auto material = static_cast<DebugMaterialResource*>(item.material.get());

                debugSurfRenderer.BindPipeline(cmd, material->GetPipelineState());
                debugSurfRenderer.Draw(cmd, *mesh, item.world);
            }
        };
}