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

void SkyboxGraphBuilder::Build(RenderGraph& graph)
{
    auto& skybox = graph.AddGraphicsPass("Skybox");
    skybox.Write(m_backBufferResID, RGAccess::RTV);

    skybox.gpuExecute =
        [
            &swapChain = m_swapChain,
            &skyboxRenderer = m_skyboxRenderer
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto& envRes = ctx.packet->environment;
            if (!envRes || !envRes->IsReady())
                return; // 환경 없는 씬 - 스카이박스 안 그림

            swapChain.SetRenderTarget(cmd);
            swapChain.SetViewport(cmd, ctx.packet->viewport);

            skyboxRenderer.Draw(cmd, ctx.frame.camera, *envRes->GetSkybox());
        };
}