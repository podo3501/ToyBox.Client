#include "pch.h"
#include "DebugSurfaceGraphBuilder.h"
#include "Graph/RenderPass.h"
#include "Graph/RenderGraph.h"
#include "Scene/RenderScene.h"
#include "Renderer/DebugSurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/GridMaterialResource.h"

DebugSurfaceGraphBuilder::~DebugSurfaceGraphBuilder() = default;
DebugSurfaceGraphBuilder::DebugSurfaceGraphBuilder(DebugSurfaceRenderer& debugSurfRenderer, RGHandle hBb) :
    m_debugSurfRenderer{ debugSurfRenderer },
    m_hBb{ hBb }
{}

void DebugSurfaceGraphBuilder::Build(RenderGraph& graph)
{
    auto& grid = graph.AddPass("DebugSurface", CommandType::Direct);
    grid.dependsOn.push_back("Opaque");
    grid.writes.push_back({ m_hBb, RGAccess::RTV });
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
                auto material = static_cast<GridMaterialResource*>(item.material.get());

                debugSurfRenderer.BindPipeline(cmd, material->GetPipelineState());
                debugSurfRenderer.Draw(cmd, *mesh, item.world);
            }
        };
}