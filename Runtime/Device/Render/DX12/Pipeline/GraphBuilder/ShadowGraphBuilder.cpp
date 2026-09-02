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
    ShadowResource& shadowRes ) noexcept :
    m_shadowRenderer{ shadowRenderer },
    m_descFactory{ descFactory },
    m_shadowRes{ shadowRes }
{}

void ShadowGraphBuilder::Build(
    RenderGraph& graph,
    RGResourceID shadowResID,
    const DirectionalLightData& light,
    std::vector<RenderShadowCasterItem> shadowCasters)
{
    auto& shadow = graph.AddGraphicsPass("Shadow");
    shadow.Write(shadowResID, RGAccess::DepthWrite);

    shadow.execute =
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

            shadowRenderer.PrepareDraw(cmd, light);

            for (auto& item : shadowCasters)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                shadowRenderer.Draw(cmd, *mesh, item.world);
            }
        };
}
