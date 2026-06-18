#pragma once
#include "Graph/RenderGraph.h"
#include "Resource/ShadowResource.h"

class Renderers;
class DescriptorFactory;
class SwapChainPresenter;

class ForwardRenderPipeline
{
public:
    ~ForwardRenderPipeline();
    ForwardRenderPipeline(SwapChainPresenter& swapChain, DescriptorFactory& descFactory);
    bool Initialize(Device& device, const Size& shadowMapSize, Renderers& renderers);
    void Render(CommandList& cmd, const DrawPacket& drawPacket, const FrameData& frame);

private:
    SwapChainPresenter& m_swapChain;
    DescriptorFactory& m_descFactory;

    RenderGraph m_graph;
    ShadowResource m_shadowRes; //이 클래스는 framereseource 클래스중의 하나. 프레임당 render가 필요한 리소스들.

    RGHandle m_hBackBuffer;
    RGHandle m_hShadow;

    std::vector<CompiledTask> m_compiledTasks;
};
