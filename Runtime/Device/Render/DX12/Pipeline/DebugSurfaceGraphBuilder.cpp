#include "pch.h"
#include "DebugSurfaceGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Scene/RenderScene.h"
#include "Renderer/DebugSurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/DebugMaterialResource.h"

DebugSurfaceGraphBuilder::~DebugSurfaceGraphBuilder() = default;
DebugSurfaceGraphBuilder::DebugSurfaceGraphBuilder(
    DebugSurfaceRenderer& debugSurfRenderer, 
    RGResourceID backBufferResID) :
    m_debugSurfRenderer{ debugSurfRenderer },
    m_backBufferResID{ backBufferResID }
{}

void DebugSurfaceGraphBuilder::Build(RenderGraph& graph)
{
    auto& grid = graph.AddGraphicsPass("DebugSurface");
    grid.Write(m_backBufferResID, RGAccess::RTV);
    grid.gpuExecute =
        [
            &debugSurfRenderer = m_debugSurfRenderer
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            debugSurfRenderer.PrepareFrame(ctx.frame.camera);
            debugSurfRenderer.BeginFrame(cmd);

            for (auto& item : ctx.drawPacket.debugSurface)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                auto material = static_cast<DebugMaterialResource*>(item.material.get());

                debugSurfRenderer.BindPipeline(cmd, material->GetPipelineState());
                debugSurfRenderer.Draw(cmd, *mesh, item.world);
            }
        };
}