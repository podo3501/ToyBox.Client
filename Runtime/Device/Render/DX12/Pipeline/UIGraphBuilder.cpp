#include "pch.h"
#include "UIGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Renderer/UIRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Brush/BrushResource.h"

UIGraphBuilder::~UIGraphBuilder() = default;
UIGraphBuilder::UIGraphBuilder(UIRenderer& uiRenderer, RGResourceID backBufferResID) :
    m_uiRenderer{ uiRenderer },
    m_backBufferResID{ backBufferResID }
{}

void UIGraphBuilder::Build(RenderGraph& graph)
{
    auto& ui = graph.AddGraphicsPass("UI");
    ui.Write(m_backBufferResID, RGAccess::RTV);
    ui.gpuExecute =
        [
            &uiRenderer = m_uiRenderer
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            uiRenderer.BeginFrame(cmd);

            for (auto& uiItem : ctx.drawPacket.ui)
            {
                auto mesh = static_cast<MeshResource*>(uiItem.mesh.get());
                auto brush = static_cast<BrushResource*>(uiItem.brush.get());

                uiRenderer.Draw(cmd, *mesh, *brush, uiItem.world, uiItem.uvTransform);
            }
        };
}