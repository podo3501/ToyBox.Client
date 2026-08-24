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
    DescriptorFactory& descFactory,
    ShadowResource& shadowRes,
    RGResourceID shadowResID) :
    m_surfRenderer{ surfRenderer },
    m_descFactory{ descFactory },
    m_shadowRes{ shadowRes },
    m_shadowResID{ shadowResID }
{}

void OpaqueGraphBuilder::Build(
    RenderGraph& graph,
    const DirectionalLightData& light,
    std::shared_ptr<ViewPacket> packet,
    size_t viewIndex,
    const ViewTargetResource& target)
{
    auto& opaque = graph.AddGraphicsPass("Opaque_View" + std::to_string(viewIndex));
    opaque.Read(m_shadowResID, RGAccess::SRV);
    opaque.Write(target.GetColorID(), RGAccess::RTV);
    opaque.Write(target.GetDepthID(), RGAccess::DepthWrite);
    opaque.gpuExecute =
        [
            &descFactory = m_descFactory,
            &surfRenderer = m_surfRenderer,
            &shadowRes = m_shadowRes,
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

            surfRenderer.PrepareFrame(
                light,
                packet->camera,
                shadowRes.GetSRVIndex(),
                packet->environment.get()
            );
            surfRenderer.BeginFrame(cmd);

            for (auto& item : packet->surface)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                auto material = static_cast<MaterialResource*>(item.material.get());
                
                surfRenderer.BindPipeline(cmd, item.pipelineState);
                surfRenderer.Draw(cmd, *mesh, *material, item.world);
            }
        };
}