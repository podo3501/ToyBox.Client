#pragma once

class Renderers;
class DescriptorFactory;
class RenderGraph;
class RenderScene;
class SwapChainPresenter;
class ShadowResource;

class ForwardRenderPipeline
{
public:
    ~ForwardRenderPipeline();
    ForwardRenderPipeline(Renderers* renderers, DescriptorFactory* descFactory);
    void BuildFrame(RenderGraph& graph, RenderScene* scene, SwapChainPresenter* swapChain, ShadowResource* shadowRes);

private:
    Renderers* m_renderers{ nullptr };
    DescriptorFactory* m_descFactory{ nullptr };
};
