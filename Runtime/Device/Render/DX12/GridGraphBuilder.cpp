#include "pch.h"
#include "GridGraphBuilder.h"
#include "RenderPass.h"
#include "RenderGraph.h"
#include "RenderScene.h"
#include "Renderer/GridRenderer.h"
#include "MeshResource.h"
#include "MeshMaterialResource.h"

GridGraphBuilder::~GridGraphBuilder() = default;
GridGraphBuilder::GridGraphBuilder(GridRenderer* gridRenderer, RenderScene* scene, RGHandle hBb) :
    m_gridRenderer{ gridRenderer },
    m_scene{ scene },
    m_hBb{ hBb }
{}

void GridGraphBuilder::Build(RenderGraph& graph)
{
    auto& grid = graph.AddPass("Grid", CommandType::Direct);
    grid.dependsOn.push_back("Opaque");
    grid.writes.push_back({ m_hBb, RGAccess::RTV });
    grid.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        m_gridRenderer->BindCommonState(cmd);
        m_gridRenderer->PrepareFrame(ctx.frame.camera);

        std::optional<PipelineState> currentPSO;
        for (auto& item : m_scene->GetGridDraws())
        {
            auto mesh = static_cast<MeshResource*>(item.mesh.get());
            auto material = static_cast<MeshMaterialResource*>(item.material.get());

            const PipelineState& nextPSO = material->GetPipelineState();
            if (!currentPSO || *currentPSO != nextPSO)
            {
                m_gridRenderer->BindPipeline(cmd, nextPSO);
                currentPSO = nextPSO;
            }

            m_gridRenderer->Draw(cmd, *mesh, item.world);
        }
        };
}