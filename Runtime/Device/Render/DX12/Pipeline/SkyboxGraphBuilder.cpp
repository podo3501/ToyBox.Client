#include "pch.h"
#include "SkyboxGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "Renderer/SkyboxRenderer.h"

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
            swapChain.SetRenderTarget(cmd);
            swapChain.Clear(cmd, 0.13f, 0.13f, 0.16f, 1.0f); // 여기로 이동

            auto& envRes = ctx.drawPacket.environment;
            if (!envRes)
                return; // 환경 없는 씬 - 스카이박스 안 그림

            if (!envRes->IsReady())
                return;

            swapChain.SetRenderTarget(cmd);
            skyboxRenderer.Draw(cmd, ctx.frame.camera, *envRes->GetSkybox());
        };
}