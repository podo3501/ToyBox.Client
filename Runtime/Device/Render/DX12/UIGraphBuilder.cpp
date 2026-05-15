#include "pch.h"
#include "UIGraphBuilder.h"
#include "RenderPass.h"
#include "RenderGraph.h"
#include "RenderScene.h"
#include "QuadRenderer.h"
#include "TextureResource.h"

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
        m_quadRenderer->BindDescriptorHeap(cmd);
        m_quadRenderer->BindPipeline(cmd);

        for (auto& item : m_scene->GetUIDraws())
        {
            auto texRes = static_cast<TextureResource*>(item.texture.get());
            m_quadRenderer->BindTexture(cmd, texRes->GetSrv());
            m_quadRenderer->Draw(cmd, item.dest);
        }
        };
}