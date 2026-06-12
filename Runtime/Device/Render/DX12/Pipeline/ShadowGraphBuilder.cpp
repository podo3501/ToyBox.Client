#include "pch.h"
#include "ShadowGraphBuilder.h"
#include "Scene/RenderScene.h"
#include "Graph/RenderPass.h"
#include "Graph/RenderGraph.h"
#include "Renderer/ShadowRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Helpers/CommandListHelpers.h"
#include "Descriptor/DescriptorFactory.h"
#include "Command/CommandList.h"
#include "Resource/ShadowResource.h"

ShadowGraphBuilder::~ShadowGraphBuilder() = default;

ShadowGraphBuilder::ShadowGraphBuilder(
    ShadowRenderer* shadowRenderer, 
    DescriptorFactory* descFactory,
    ShadowResource* shadowRes,
    RenderScene* scene, RGHandle hShadow) :
    m_shadowRenderer{ shadowRenderer },
    m_descFactory{ descFactory },
    m_shadowRes{ shadowRes },
    m_scene{ scene },
    m_hShadow{ hShadow }
{}

void ShadowGraphBuilder::Build(RenderGraph& graph)
{
    auto& shadow = graph.AddPass("Shadow", CommandType::Direct);
    shadow.writes.push_back({ m_hShadow, RGAccess::DepthWrite });
    shadow.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        auto dsv = m_descFactory->GetDsvHandle(m_shadowRes->GetDsvIndex());

        CommandUtils::SetViewport(cmd, 2048.f, 2048.f);
        CommandUtils::SetScissor(cmd, 2048, 2048);

        CommandUtils::ClearDSV(cmd, dsv);
        CommandUtils::SetDepthTarget(cmd, dsv);

        m_shadowRenderer->BindCommonState(cmd);
        m_shadowRenderer->PrepareFrame(ctx.frame.light);

        for (auto& item : m_scene->GetSurfaceDraws())
        {
            auto mesh = static_cast<MeshResource*>(item.mesh.get());

            m_shadowRenderer->BindPipeline(cmd);
            m_shadowRenderer->Draw(cmd, *mesh, item.world);
        }
        };
}
