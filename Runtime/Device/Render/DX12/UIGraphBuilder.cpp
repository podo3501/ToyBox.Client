#include "pch.h"
#include "UIGraphBuilder.h"
#include "RenderPass.h"
#include "RenderGraph.h"
#include "RenderScene.h"
#include "QuadRenderer.h"
#include "MeshResource.h"
#include "UIMaterialResource.h"

UIGraphBuilder::~UIGraphBuilder() = default;
UIGraphBuilder::UIGraphBuilder(QuadRenderer* quadRenderer, RenderScene* scene, RGHandle hBb) :
    m_quadRenderer{ quadRenderer },
    m_scene{ scene }, 
    m_hBb{ hBb }
{}

void UIGraphBuilder::Build(RenderGraph& graph)
{
    auto& ui = graph.AddPass("UI", CommandType::Direct);
    ui.dependsOn.push_back("Opaque");
    ui.writes.push_back({ m_hBb, RGAccess::RTV });
    ui.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        m_quadRenderer->BindCommonState(cmd);
        m_quadRenderer->PrepareFrame();

        std::optional<PipelineState> currentPSO;
        for (auto& item : m_scene->GetUIDraws())
        {
            auto mesh = static_cast<MeshResource*>(item.mesh.get());
            auto material = static_cast<UIMaterialResource*>(item.material.get());

            const PipelineState& nextPSO = material->GetPipelineState();
            if (!currentPSO || *currentPSO != nextPSO)
            {
                m_quadRenderer->BindPipeline(cmd, nextPSO);
                currentPSO = nextPSO;
            }

            m_quadRenderer->Draw(cmd, *mesh, *material, item.world);
        }
        };
}