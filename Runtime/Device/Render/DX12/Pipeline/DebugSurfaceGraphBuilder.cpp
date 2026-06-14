#include "pch.h"
#include "DebugSurfaceGraphBuilder.h"
#include "Graph/RenderPass.h"
#include "Graph/RenderGraph.h"
#include "Scene/RenderScene.h"
#include "Renderer/DebugSurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/GridMaterialResource.h"

DebugSurfaceGraphBuilder::~DebugSurfaceGraphBuilder() = default;
DebugSurfaceGraphBuilder::DebugSurfaceGraphBuilder(DebugSurfaceRenderer* debugSurfRenderer, RenderScene* scene, RGHandle hBb) :
    m_debugSurfRenderer{ debugSurfRenderer },
    m_scene{ scene },
    m_hBb{ hBb }
{}

void DebugSurfaceGraphBuilder::Build(RenderGraph& graph)
{
    auto& grid = graph.AddPass("DebugSurface", CommandType::Direct);
    grid.dependsOn.push_back("Opaque");
    grid.writes.push_back({ m_hBb, RGAccess::RTV });
    grid.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        m_debugSurfRenderer->BindCommonState(cmd);
        m_debugSurfRenderer->PrepareFrame(ctx.frame.camera);

        for (auto& item : m_scene->GetDrawList(MaterialDomain::DebugSurface))
        {
            auto mesh = static_cast<MeshResource*>(item.mesh.get());
            auto material = static_cast<GridMaterialResource*>(item.material.get());

            m_debugSurfRenderer->BindPipeline(cmd, material->GetPipelineState());
            m_debugSurfRenderer->Draw(cmd, *mesh, item.world);
        }
        };
}