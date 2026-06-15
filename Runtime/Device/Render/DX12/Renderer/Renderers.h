#pragma once
#include <d3d12.h>
#include "PipelineCache.h"
#include "RendererConfig.h"

struct Size;
class Device;
class ShaderProvider;
class ShadowRenderer;
class SurfaceRenderer;
class DebugSurfaceRenderer;
class UIRenderer;

class Renderers
{
public:
    ~Renderers();
    Renderers(Device& device, ShaderProvider* shaderProvider);
    bool Initialize(const Size& screenSize, ID3D12DescriptorHeap* srvHeap);
    void SetScreenSize(const Size& screenSize);

    ShadowRenderer* GetShadowRenderer() const { return m_shadowRenderer.get(); }
    SurfaceRenderer* GetSurfRenderer() const { return m_surfRenderer.get(); }
    DebugSurfaceRenderer* GetDebugSurfRenderer() const { return m_debugSurfRenderer.get(); }
    UIRenderer* GetUIRenderer() const { return m_uiRenderer.get(); }

private:
    Device& m_device;
    RendererConfig m_config;
    PipelineCache m_pipelineCache;

    std::unique_ptr<ShadowRenderer> m_shadowRenderer;
    std::unique_ptr<SurfaceRenderer> m_surfRenderer;
    std::unique_ptr<DebugSurfaceRenderer> m_debugSurfRenderer;
    std::unique_ptr<UIRenderer> m_uiRenderer;
};