#include "pch.h"
#include "InspectorGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Inspector/InspectorImageRenderer.h"

InspectorGraphBuilder::~InspectorGraphBuilder() = default;
InspectorGraphBuilder::InspectorGraphBuilder(InspectorImageRenderer& imageRenderer, RGResourceID backBufferResID) :
    m_imageRenderer{ imageRenderer },
    m_backBufferResID{ backBufferResID }
{}

void InspectorGraphBuilder::Build(RenderGraph& graph)
{
    auto& inspector = graph.AddGraphicsPass("Inspector");
    inspector.Write(m_backBufferResID, RGAccess::RTV);
    inspector.gpuExecute =
        [
            &imageInspector = m_imageRenderer
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            imageInspector.PrepareFrame();
            imageInspector.BeginFrame(cmd);

            //for (auto& item : ctx.packet->debug.images)
            //{
            //    imageInspector.BindPipeline(cmd);
            //    imageInspector.Draw(cmd, *item.texture);
            //}
        };
}