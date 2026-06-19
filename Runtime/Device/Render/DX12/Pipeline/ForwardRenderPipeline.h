#pragma once
#include "Graph/RenderGraph.h"
#include "Resource/ShadowResource.h"
#include "Renderer/Renderers.h"

class Device;
class ShaderLibrary;
class DescriptorFactory;
class SwapChainPresenter;

class ForwardRenderPipeline
{
public:
    ~ForwardRenderPipeline();
    ForwardRenderPipeline(
        Device& device,
        SwapChainPresenter& swapChain,
        DescriptorFactory& descFactory,
        ShaderLibrary& shaderLibrary);
    bool Initialize(const Size& screenSize, const Size& shadowMapSize);
    void Render(CommandList& cmd, const DrawPacket& drawPacket, const FrameData& frame);
    void Resize(const Size& size);

private:
    Device& m_device;
    SwapChainPresenter& m_swapChain;
    DescriptorFactory& m_descFactory;

    RenderGraph m_graph;
    ShadowResource m_shadowRes; //이 클래스는 framereseource 클래스중의 하나. 프레임당 render가 필요한 리소스들.
    Renderers m_renderers;

    RGHandle m_hBackBuffer;
    RGHandle m_hShadow;

    std::vector<CompiledTask> m_compiledTasks;
};
