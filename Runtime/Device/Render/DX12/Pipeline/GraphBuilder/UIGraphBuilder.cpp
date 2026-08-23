#include "pch.h"
#include "UIGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include "Factory/DescriptorFactory.h"
#include "../Renderer/UIRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Brush/BrushResource.h"
#include "Resource/Internal/ViewTargetResource.h"

UIGraphBuilder::~UIGraphBuilder() = default;
UIGraphBuilder::UIGraphBuilder(
    UIRenderer& uiRenderer, 
    DescriptorFactory& descFactory) :
    m_uiRenderer{ uiRenderer },
    m_descFactory{ descFactory }
{}

void UIGraphBuilder::Build(
    RenderGraph& graph,
    std::shared_ptr<ViewPacket> packet,
    size_t viewIndex,
    const ViewTargetResource& target)
{
    auto& ui = graph.AddGraphicsPass("UI_View" + std::to_string(viewIndex));
    ui.Write(target.GetColorID(), RGAccess::RTV);
    ui.gpuExecute =
        [
            &descFactory = m_descFactory,
            &uiRenderer = m_uiRenderer,
            packet,
            colorRTVIndex = target.GetColorRTVIndex()
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto rtv = descFactory.GetRTVHandle(colorRTVIndex);

            CommandUtils::SetRenderTarget(cmd, rtv);
            CommandUtils::SetViewport(
                cmd,
                packet->viewport.x,
                packet->viewport.y,
                packet->viewport.width,
                packet->viewport.height);

            uiRenderer.BeginFrame(cmd);

            for (auto& uiItem : packet->ui)
            {
                auto mesh = static_cast<MeshResource*>(uiItem.mesh.get());
                auto brush = static_cast<BrushResource*>(uiItem.brush.get());

                uiRenderer.Draw(cmd, *mesh, *brush, uiItem.world, packet->uiCamera.proj, uiItem.source);
            }
        };
}