#pragma once
#include "Resource/ShadowResource.h"
#include "Renderer/Renderers.h"
#include "Graph/RGTypes.h"

struct FrameData;
struct DrawPacket;
struct CompiledTask;
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

    ShadowResource m_shadowRes; //이 클래스는 framereseource 클래스중의 하나. 프레임당 render가 필요한 리소스들.
    Renderers m_renderers;

    RGResourceID m_hBackBuffer;
    RGResourceID m_hShadow;

    std::vector<CompiledTask> m_compiledTasks;
};
