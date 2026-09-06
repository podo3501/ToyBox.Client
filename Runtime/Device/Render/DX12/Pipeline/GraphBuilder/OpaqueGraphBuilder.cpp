#include "pch.h"
#include "OpaqueGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include "Factory/DescriptorFactory.h"
#include "../Renderer/SurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/MaterialResource.h"
#include "Resource/Internal/ShadowResource.h"
#include "Resource/Internal/ViewTargetResource.h"

OpaqueGraphBuilder::~OpaqueGraphBuilder() = default;
OpaqueGraphBuilder::OpaqueGraphBuilder(
    SurfaceRenderer& surfRenderer, 
    DescriptorFactory& descFactory) :
    m_surfRenderer{ surfRenderer },
    m_descFactory{ descFactory }
{}

void OpaqueGraphBuilder::Build(
    RenderGraph& graph,
    const DirectionalLightData& light,
    const ShadowResource& shadowRes,
    RGResourceID shadowResID,
    std::shared_ptr<ViewPacket> packet,
    const ViewTargetResource& target)
{
    auto& opaque = graph.AddGraphicsPass("Opaque_View" + std::to_string(packet->id));
    opaque.Read(shadowResID, RGAccess::SRV);
    opaque.Write(target.GetColorID(), RGAccess::RTV);
    opaque.Write(target.GetDepthID(), RGAccess::DepthWrite);
    opaque.execute =
        [
            &descFactory = m_descFactory,
            &surfRenderer = m_surfRenderer,
            &shadowRes = shadowRes,
            light,
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

            surfRenderer.PrepareDraw(
                cmd, 
                light,
                packet->camera,
                shadowRes.GetSRVIndex(),
                packet->environment.get());

            for (auto& item : packet->surface)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                auto material = static_cast<MaterialResource*>(item.material.get());
                
                surfRenderer.BindPipeline(cmd, item.pipelineState);
                surfRenderer.Draw(cmd, *mesh, *material, item.world);
            }
        };
}