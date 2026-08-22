#include "pch.h"
#include "ShadowGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "../Renderer/ShadowRenderer.h"
#include "Command/CommandListHelpers.h"
#include "Command/CommandList.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Internal/ShadowResource.h"
#include "Factory/DescriptorFactory.h"

ShadowGraphBuilder::~ShadowGraphBuilder() = default;

ShadowGraphBuilder::ShadowGraphBuilder(
    ShadowRenderer& shadowRenderer, 
    DescriptorFactory& descFactory,
    ShadowResource& shadowRes,
    RGResourceID shadowResID) :
    m_shadowRenderer{ shadowRenderer },
    m_descFactory{ descFactory },
    m_shadowRes{ shadowRes },
    m_shadowResID{ shadowResID }
{}

void ShadowGraphBuilder::Build(
    RenderGraph& graph,
    const DirectionalLightData& light,
    std::vector<RenderShadowCasterItem> shadowCasters)
{
    auto& shadow = graph.AddGraphicsPass("Shadow");
    shadow.Write(m_shadowResID, RGAccess::DepthWrite);

    shadow.gpuExecute =
        [
            &shadowRenderer = m_shadowRenderer,
            &descFactory = m_descFactory,
            &shadowRes = m_shadowRes,
            shadowCasters = std::move(shadowCasters),
            light
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto dsv = descFactory.GetDSVHandle(shadowRes.GetDSVIndex());

            CommandUtils::SetViewport(cmd, 0.f, 0.f, 2048.f, 2048.f);
            CommandUtils::SetScissor(cmd, 0, 0, 2048, 2048);

            CommandUtils::ClearDSV(cmd, dsv);
            CommandUtils::SetDepthTarget(cmd, dsv);

            shadowRenderer.PrepareFrame(light);
            shadowRenderer.BeginFrame(cmd);

            for (auto& item : shadowCasters)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                shadowRenderer.Draw(cmd, *mesh, item.world);
            }
        };
}
