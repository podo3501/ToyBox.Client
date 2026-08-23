#include "pch.h"
#include "SkyboxGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include "Factory/DescriptorFactory.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "Resource/Internal/ViewTargetResource.h"
#include "../Renderer/SkyboxRenderer.h"

SkyboxGraphBuilder::~SkyboxGraphBuilder() = default;
SkyboxGraphBuilder::SkyboxGraphBuilder(
    SkyboxRenderer& skyboxRenderer,
    DescriptorFactory& descFactory) :
    m_skyboxRenderer{ skyboxRenderer },
    m_descFactory{ descFactory }
{}

void SkyboxGraphBuilder::Build(
    RenderGraph& graph, 
    std::shared_ptr<ViewPacket> packet, 
    size_t viewIndex,
    const ViewTargetResource& target)
{
    auto& skybox = graph.AddGraphicsPass("Skybox_View" + std::to_string(viewIndex));
    skybox.Write(target.GetColorID(), RGAccess::RTV);
    skybox.Write(target.GetDepthID(), RGAccess::DepthWrite);

    skybox.gpuExecute =
        [
            &descFactory = m_descFactory,
            &skyboxRenderer = m_skyboxRenderer,
            packet,
            colorRTVIndex = target.GetColorRTVIndex(),
            depthDSVIndex = target.GetDepthDSVIndex()
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto& envRes = packet->environment;
            if (!envRes || !envRes->IsReady())
                return; // 환경 없는 씬 - 스카이박스 안 그림

            auto rtv = descFactory.GetRTVHandle(colorRTVIndex);
            auto dsv = descFactory.GetDSVHandle(depthDSVIndex);

            CommandUtils::SetRenderTarget(cmd, rtv, dsv);
            CommandUtils::SetViewport(
                cmd, 
                packet->viewport.x, 
                packet->viewport.y, 
                packet->viewport.width,
                packet->viewport.height);

            skyboxRenderer.Draw(cmd, packet->camera, *envRes->GetSkybox());
        };
}