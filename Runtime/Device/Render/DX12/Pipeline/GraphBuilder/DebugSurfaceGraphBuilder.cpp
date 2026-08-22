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

void DebugSurfaceGraphBuilder::Build(
    RenderGraph& graph,
    std::shared_ptr<ViewPacket> packet,
    size_t viewIndex)
{
    auto& grid = graph.AddGraphicsPass("DebugSurface_View" + std::to_string(viewIndex));
    grid.Write(m_backBufferResID, RGAccess::RTV);
    grid.gpuExecute =
        [
            &swapChain = m_swapChain,
            &debugSurfRenderer = m_debugSurfRenderer,
            packet
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);
            swapChain.SetViewport(cmd, packet->viewport);

            debugSurfRenderer.PrepareFrame(packet->camera);
            debugSurfRenderer.BeginFrame(cmd);

            for (auto& item : packet->debugSurface)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                auto material = static_cast<DebugMaterialResource*>(item.material.get());

                debugSurfRenderer.BindPipeline(cmd, material->GetPipelineState());
                debugSurfRenderer.Draw(cmd, *mesh, item.world);
            }
        };
}