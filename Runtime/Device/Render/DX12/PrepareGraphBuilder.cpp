#include "pch.h"
#include "PrepareGraphBuilder.h"
#include "RenderPass.h"
#include "RenderGraph.h"
#include "SwapChainPresenter.h"
#include "CommandUtils.h"
#include "CommandList.h"

PrepareGraphBuilder::PrepareGraphBuilder(SwapChainPresenter* swapChain, RGHandle hBb) :
	m_swapChain{ swapChain },
    m_hBb{ hBb }
{}

void PrepareGraphBuilder::Build(RenderGraph& graph)
{
    auto& prepare = graph.AddPass("Prepare", CommandType::Direct);
    prepare.writes.push_back({ m_hBb, RGAccess::RTV });
    prepare.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        m_swapChain->SetRenderTarget(cmd);
        m_swapChain->Clear(cmd, 0.13f, 0.13f, 0.16f, 1.0f);
        };
}