#pragma once
#include "Graph/RenderGraph.h"

class Renderers;
class DescriptorFactory;
class SwapChainPresenter;
class ShadowResource;

class ForwardRenderPipeline
{
public:
    ~ForwardRenderPipeline();
    ForwardRenderPipeline(Renderers* renderers, SwapChainPresenter* swapChain,
        DescriptorFactory* descFactory, ShadowResource* shadowRes);
    void Render(CommandList& cmd, const DrawPacket& drawPacket, const FrameData& frame);

private:
    void BuildGraph(Renderers* renderers, DescriptorFactory* descFactory);

    SwapChainPresenter* m_swapChain{ nullptr };
    ShadowResource* m_shadowRes{ nullptr };

    RenderGraph m_graph;

    RGHandle m_hBackBuffer;
    RGHandle m_hShadow;

    std::vector<CompiledTask> m_compiledTasks;
};
