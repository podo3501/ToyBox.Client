#include "pch.h"
#include "OpaqueGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include "Factory/DescriptorFactory.h"
#include "Pipeline/Renderer/SurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/MaterialResource.h"
#include "Resource/Internal/ShadowResource.h"
#include "Resource/Internal/ViewTargetResource.h"
#include "Definition/RenderData.h"
#include "Task/Types/TaskCommandLists.h"

OpaqueGraphBuilder::~OpaqueGraphBuilder() = default;
OpaqueGraphBuilder::OpaqueGraphBuilder(
    SurfaceRenderer& surfRenderer, 
    DescriptorFactory& descFactory) :
    m_surfRenderer{ surfRenderer },
    m_descFactory{ descFactory },
    m_recordPool{ 10 }
{}

void OpaqueGraphBuilder::Build(
    RenderGraph& graph,
    const DirectionalLightData& light,
    const ShadowResource& shadowRes,
    RGResourceID shadowResID,
    std::shared_ptr<SceneViewPacket> packet,
    const ViewTargetResource& target)
{
    Assert(!packet->surface.empty());

    auto& opaque = graph.AddGraphicsPass("Opaque_View" + std::to_string(packet->target.id));
    opaque.Read(shadowResID, RGAccess::SRV);
    opaque.Write(target.GetColorID(), RGAccess::RTV);
    opaque.Write(target.GetDepthID(), RGAccess::DepthWrite);
    opaque.numParallel = static_cast<uint32_t>(
        std::min<size_t>(3, std::max<size_t>(1, packet->surface.size())));

    opaque.execute =
        [
            &descFactory = m_descFactory,
            &surfRenderer = m_surfRenderer,
            &shadowRes = shadowRes,
            &recordPool = m_recordPool,
            light,
            packet,
            colorRTVIndex = target.GetColorRTVIndex(),
            depthDSVIndex = target.GetDepthDSVIndex()
        ]
        (TaskCommandLists cmds, TaskContext& ctx)
        {
            auto rtv = descFactory.GetRTVHandle(colorRTVIndex);
            auto dsv = descFactory.GetDSVHandle(depthDSVIndex);

            const size_t total = packet->surface.size();
            const size_t actual = std::min(cmds.Size(), total);
            const size_t chunkSize = (total + actual - 1) / actual;

            recordPool.ExecuteParallel(actual, [&](size_t t) 
            {
                CommandList& cmd = cmds[t];
                CommandUtils::SetRenderTarget(cmd, rtv, dsv);
                CommandUtils::SetViewRect(cmd, packet->target.localViewport);
                surfRenderer.PrepareDraw(
                    cmd, light, packet->target.camera,
                    shadowRes.GetSRVIndex(), packet->environment.get());

                const size_t begin = t * chunkSize;
                const size_t end = std::min(begin + chunkSize, total);
                for (size_t i = begin; i < end; ++i)
                {
                    auto& item = packet->surface[i];
                    auto mesh = static_cast<MeshResource*>(item.mesh.get());
                    auto material = static_cast<MaterialResource*>(item.material.get());
                    surfRenderer.BindPipeline(cmd, item.pipelineState);
                    surfRenderer.Draw(cmd, *mesh, *material, item.world);
                }
            });
        };
}