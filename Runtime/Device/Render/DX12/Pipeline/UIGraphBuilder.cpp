#include "pch.h"
#include "UIGraphBuilder.h"
#include "Graph/RenderPass.h"
#include "Graph/RenderGraph.h"
#include "Renderer/UIRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/UIMaterialResource.h"

UIGraphBuilder::~UIGraphBuilder() = default;
UIGraphBuilder::UIGraphBuilder(UIRenderer& uiRenderer, RGHandle hBb) :
    m_uiRenderer{ uiRenderer },
    m_hBb{ hBb }
{}

void UIGraphBuilder::Build(RenderGraph& graph)
{
    auto& ui = graph.AddPass("UI", CommandType::Direct);
    ui.dependsOn.push_back("DebugSurface");
    ui.writes.push_back({ m_hBb, RGAccess::RTV });
    ui.gpuExecute =
        [
            &uiRenderer = m_uiRenderer
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            uiRenderer.BindRootSignature(cmd);
            uiRenderer.PrepareFrame();

            for (auto& item : ctx.drawPacket.ui)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                auto material = static_cast<UIMaterialResource*>(item.material.get());

                uiRenderer.BindPipeline(cmd, material->GetPipelineState());
                uiRenderer.Draw(cmd, *mesh, *material, item.world);
            }
        };
}