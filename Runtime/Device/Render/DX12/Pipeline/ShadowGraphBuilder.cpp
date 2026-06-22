#include "pch.h"
#include "ShadowGraphBuilder.h"
#include "Graph/RenderPass.h"
#include "Graph/RenderGraph.h"
#include "Renderer/ShadowRenderer.h"
#include "Command/CommandListHelpers.h"
#include "Command/CommandList.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/ShadowResource.h"
#include "Factory/DescriptorFactory.h"

ShadowGraphBuilder::~ShadowGraphBuilder() = default;

ShadowGraphBuilder::ShadowGraphBuilder(
    ShadowRenderer& shadowRenderer, 
    DescriptorFactory& descFactory,
    ShadowResource& shadowRes,
    RGHandle hShadow) :
    m_shadowRenderer{ shadowRenderer },
    m_descFactory{ descFactory },
    m_shadowRes{ shadowRes },
    m_hShadow{ hShadow }
{}

void ShadowGraphBuilder::Build(RenderGraph& graph)
{
    auto& shadow = graph.AddPass("Shadow", CommandType::Direct);
    shadow.writes.push_back({ m_hShadow, RGAccess::DepthWrite });

    shadow.gpuExecute =
        [
            &shadowRenderer = m_shadowRenderer,
            &descFactory = m_descFactory,
            &shadowRes = m_shadowRes
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto dsv = descFactory.GetDSVHandle(shadowRes.GetDSVIndex());

            CommandUtils::SetViewport(cmd, 2048.f, 2048.f);
            CommandUtils::SetScissor(cmd, 2048, 2048);

            CommandUtils::ClearDSV(cmd, dsv);
            CommandUtils::SetDepthTarget(cmd, dsv);

            shadowRenderer.PrepareFrame(ctx.frame.light);
            shadowRenderer.BeginFrame(cmd);

            for (auto& item : ctx.drawPacket.surface)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                shadowRenderer.Draw(cmd, *mesh, item.world);
            }
        };
}
