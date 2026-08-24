#include "pch.h"
#include "DebugSurfaceGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include "Factory/DescriptorFactory.h"
#include "../Renderer/DebugSurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/DebugMaterialResource.h"
#include "Resource/Internal/ViewTargetResource.h"

DebugSurfaceGraphBuilder::~DebugSurfaceGraphBuilder() = default;
DebugSurfaceGraphBuilder::DebugSurfaceGraphBuilder(
    DebugSurfaceRenderer& debugSurfRenderer, 
    DescriptorFactory& descFactory) :
    m_debugSurfRenderer{ debugSurfRenderer },
    m_descFactory{ descFactory }
{}

void DebugSurfaceGraphBuilder::Build(
    RenderGraph& graph,
    std::shared_ptr<ViewPacket> packet,
    size_t viewIndex,
    const ViewTargetResource& target)
{
    auto& grid = graph.AddGraphicsPass("DebugSurface_View" + std::to_string(viewIndex));
    grid.Write(target.GetColorID(), RGAccess::RTV);
    grid.Read(target.GetDepthID(), RGAccess::DepthRead);
    grid.gpuExecute =
        [
            &descFactory = m_descFactory,
            &debugSurfRenderer = m_debugSurfRenderer,
            packet,
            colorRTVIndex = target.GetColorRTVIndex(),
            depthDSVIndex = target.GetDepthDSVIndex()
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto rtv = descFactory.GetRTVHandle(colorRTVIndex);
            auto dsv = descFactory.GetDSVHandle(depthDSVIndex);

            CommandUtils::SetRenderTarget(cmd, rtv, dsv);
            CommandUtils::SetViewRect(cmd, packet->localViewport);

            debugSurfRenderer.PrepareFrame(packet->camera);
            debugSurfRenderer.BeginFrame(cmd);

            for (auto& item : packet->debugSurface)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                auto material = static_cast<DebugMaterialResource*>(item.material.get());

                debugSurfRenderer.BindPipeline(cmd, material->GetPipelineState());
                debugSurfRenderer.Draw(cmd, *mesh, item.world);
            }
        };
}