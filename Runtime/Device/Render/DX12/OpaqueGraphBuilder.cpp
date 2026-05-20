#include "pch.h"
#include "OpaqueGraphBuilder.h"
#include "RenderPass.h"
#include "RenderGraph.h"
#include "RenderScene.h"
#include "MeshRenderer.h"
#include "MeshResource.h"
#include "MeshMaterialResource.h"

OpaqueGraphBuilder::~OpaqueGraphBuilder() = default;
OpaqueGraphBuilder::OpaqueGraphBuilder(MeshRenderer* meshRenderer, RenderScene* scene, RGHandle hBb) :
    m_meshRenderer{ meshRenderer },
    m_scene{ scene }, 
    m_hBb { hBb }
{}

void OpaqueGraphBuilder::Build(RenderGraph& graph)
{
    auto& opaque = graph.AddPass("Opaque", CommandType::Direct);
    opaque.dependsOn.push_back("Prepare");
    opaque.writes.push_back({ m_hBb, RGAccess::RTV });
    opaque.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        m_meshRenderer->BindCommonState(cmd);
        m_meshRenderer->PrepareFrame(ctx.frame.light, ctx.frame.camera);

        std::optional<PipelineState> currentPSO;
        for (auto& item : m_scene->GetOpaqueDraws())
        {
            auto mesh = static_cast<MeshResource*>(item.mesh.get());
            auto material = static_cast<MeshMaterialResource*>(item.material.get());

            const PipelineState& nextPSO = material->GetPipelineState();
            if (!currentPSO || *currentPSO != nextPSO)
            {
                m_meshRenderer->BindPipeline(cmd, nextPSO);
                currentPSO = nextPSO;
            }

            m_meshRenderer->Draw(cmd, *mesh, *material, item.world);
        }
        };
}