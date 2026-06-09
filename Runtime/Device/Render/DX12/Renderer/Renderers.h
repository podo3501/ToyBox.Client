#pragma once
#include <d3d12.h>

struct Size;
class ShaderSystem;
class ShadowRenderer;
class SurfaceRenderer;
class DebugSurfaceRenderer;
class UIRenderer;

class Renderers
{
public:
    ~Renderers();
    Renderers();
    bool Initialize(
        ID3D12Device* device, 
        ShaderSystem* shaderSystem, 
        const Size& screenSize,
        ID3D12DescriptorHeap* srvHeap);
    void SetScreenSize(const Size& screenSize);

    ShadowRenderer* GetShadowRenderer() const { return m_shadowRenderer.get(); }
    SurfaceRenderer* GetSurfRenderer() const { return m_surfRenderer.get(); }
    DebugSurfaceRenderer* GetDebugSurfRenderer() const { return m_debugSurfRenderer.get(); }
    UIRenderer* GetUIRenderer() const { return m_uiRenderer.get(); }

private:
    std::unique_ptr<ShadowRenderer> m_shadowRenderer;
    std::unique_ptr<SurfaceRenderer> m_surfRenderer;
    std::unique_ptr<DebugSurfaceRenderer> m_debugSurfRenderer;
    std::unique_ptr<UIRenderer> m_uiRenderer;
};