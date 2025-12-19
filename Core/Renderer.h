#pragma once

#include "Public/IRenderer.h"
#include "External/IDeviceNotify.h"

struct IImgui;
struct IImguiRenderer;
struct IComponent;
class TextureRepository;
class TextureRenderTarget;
class CycleIterator;

namespace DX
{
    class DeviceResources;
}

namespace DirectX
{
    class ResourceUploadBatch;
}

// A basic game implementation that creates a D3D12 device and
// provides a game loop.
class Renderer final : public DX::IDeviceNotify, public IRenderer
{
    using DeviceLostListener = function<void()>;

public:

    Renderer(HWND hwnd, int width, int height, IImguiRenderer* imguiRenderer) noexcept(false);
    ~Renderer();

    Renderer(Renderer&&) = default;
    Renderer& operator= (Renderer&&) = default;

    Renderer(Renderer const&) = delete;
    Renderer& operator= (Renderer const&) = delete;

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Initialization and management
    virtual bool Initialize() override;
    virtual void SetComponentRenderer(function<void(ITextureRender*)> rendererFn) noexcept override;
    virtual bool LoadTextureBinder(ITextureBinder* textureBinder) override;

    virtual void Draw() override;
    virtual ITextureController* GetTextureController() const noexcept override;

    // Messages
    virtual void OnActivated() override;
    virtual void OnDeactivated() override;
    virtual void OnSuspending() override;
    virtual void OnResuming() override;
    virtual void OnWindowMoved() override;
    virtual void OnDisplayChange() override;
    virtual void OnWindowSizeChanged(int width, int height) override;

private:
    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    unique_ptr<DX::DeviceResources> m_deviceResources;
    unique_ptr<GraphicsMemory> m_graphicsMemory;
    unique_ptr<DescriptorHeap> m_srvDescriptors;
    unique_ptr<ResourceUploadBatch> m_batch;

    IImguiRenderer* m_imguiRenderer{ nullptr };
    unique_ptr<SpriteBatch> m_spriteBatch;
    unique_ptr<TextureRepository> m_texRepository;

    function<void(ITextureRender*)> m_componentRenderer;
};
