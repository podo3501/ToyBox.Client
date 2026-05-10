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
        Clear(cmd, 0.13f, 0.13f, 0.16f, 1.0f);
        };
}
//?!? Clear 함수가 여기 있는것보다 swapchain 안에 있는게 나을꺼 같기도..
void PrepareGraphBuilder::Clear(CommandList& cmd, float r, float g, float b, float a)
{
    auto rtv = m_swapChain->GetCurrentRTV();

    float color[4] = { r, g, b, a };
    CommandUtils::ClearRTV(cmd, rtv, color);
}