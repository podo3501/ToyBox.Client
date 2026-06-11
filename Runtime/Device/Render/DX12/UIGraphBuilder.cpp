#include "pch.h"
#include "UIGraphBuilder.h"
#include "RenderScene.h"
#include "Graph/RenderPass.h"
#include "Graph/RenderGraph.h"
#include "Renderer/UIRenderer.h"
#include "Asset/Mesh/MeshResource.h"
#include "Asset/Material/Resource/UIMaterialResource.h"

UIGraphBuilder::~UIGraphBuilder() = default;
UIGraphBuilder::UIGraphBuilder(UIRenderer* uiRenderer, RenderScene* scene, RGHandle hBb) :
    m_uiRenderer{ uiRenderer },
    m_scene{ scene }, 
    m_hBb{ hBb }
{}

void UIGraphBuilder::Build(RenderGraph& graph)
{
    auto& ui = graph.AddPass("UI", CommandType::Direct);
    ui.dependsOn.push_back("DebugSurface");
    ui.writes.push_back({ m_hBb, RGAccess::RTV });
    ui.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        m_uiRenderer->BindCommonState(cmd);
        m_uiRenderer->PrepareFrame();

        for (auto& item : m_scene->GetUIDraws())
        {
            auto mesh = static_cast<MeshResource*>(item.mesh.get());
            auto material = static_cast<UIMaterialResource*>(item.material.get());

            m_uiRenderer->BindPipeline(cmd, material->GetPipelineState());
            m_uiRenderer->Draw(cmd, *mesh, *material, item.world);
        }
        };
}