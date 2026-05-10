#pragma once
#include "RGTypes.h"

class SwapChainPresenter;
class RenderGraph;
class CommandList;

class PrepareGraphBuilder
{
public:
    ~PrepareGraphBuilder() = default;
    PrepareGraphBuilder() = delete;
    PrepareGraphBuilder(SwapChainPresenter* swapChain, RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    void Clear(CommandList& cmd, float r, float g, float b, float a);

    SwapChainPresenter* m_swapChain{ nullptr };
    RGHandle m_hBb;
};