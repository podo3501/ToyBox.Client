#include "pch.h"
#include "InspectorGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Inspector/InspectorImageRenderer.h"

InspectorGraphBuilder::~InspectorGraphBuilder() = default;
InspectorGraphBuilder::InspectorGraphBuilder(InspectorImageRenderer& imageRenderer) noexcept :
    m_imageRenderer{ imageRenderer }
{}

void InspectorGraphBuilder::Build(
    RenderGraph& graph, 
    RGResourceID backBufferResID, 
    RGResourceID resID)
{
    auto& inspector = graph.AddGraphicsPass("Inspector");
    inspector.Write(backBufferResID, RGAccess::RTV);
    inspector.Read(resID, RGAccess::SRV);
    inspector.gpuExecute =
        [
            &imageInspector = m_imageRenderer
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            imageInspector.PrepareFrame();
            imageInspector.BeginFrame(cmd);

            imageInspector.BindPipeline(cmd);
            //imageInspector.Draw(cmd, ctx.GetResource(id));

            //for (auto& item : ctx.packet->debug.images)
            //{
            //    imageInspector.BindPipeline(cmd);
            //    imageInspector.Draw(cmd, *item.texture);
            //}
        };
}