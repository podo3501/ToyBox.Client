#include "pch.h"
#include "SkyboxGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "../Renderer/SkyboxRenderer.h"

SkyboxGraphBuilder::~SkyboxGraphBuilder() = default;
SkyboxGraphBuilder::SkyboxGraphBuilder(
    SkyboxRenderer& skyboxRenderer,
    SwapChainPresenter& swapChain,
    RGResourceID backBufferResID) :
    m_skyboxRenderer{ skyboxRenderer },
    m_swapChain{ swapChain },
    m_backBufferResID{ backBufferResID }
{}

void SkyboxGraphBuilder::Build(
    RenderGraph& graph, 
    std::shared_ptr<ViewPacket> packet, 
    size_t viewIndex)
{
    auto& skybox = graph.AddGraphicsPass("Skybox_View" + std::to_string(viewIndex));
    skybox.Write(m_backBufferResID, RGAccess::RTV);

    skybox.gpuExecute =
        [
            &swapChain = m_swapChain,
            &skyboxRenderer = m_skyboxRenderer,
            packet
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto& envRes = packet->environment;
            if (!envRes || !envRes->IsReady())
                return; // 환경 없는 씬 - 스카이박스 안 그림

            swapChain.SetRenderTarget(cmd);
            swapChain.SetViewport(cmd, packet->viewport);

            skyboxRenderer.Draw(cmd, packet->camera, *envRes->GetSkybox());
        };
}