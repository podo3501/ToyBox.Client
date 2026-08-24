#pragma once
#include <wrl/client.h>
#include "Core/Foundation/Geometry2D.h"
#include "Resource/Resource.h"

struct IDXGISwapChain4;
struct ID3D12DescriptorHeap;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct Size;
class Device;
class CommandList;
class CommandScheduler;

struct SwapChainDesc
{
    HWND hwnd{};
    Size size{};
    bool allowTearing{ false };
    UINT frameCount{ 2 };
};

class SwapChainPresenter
{
public:
    ~SwapChainPresenter();
    SwapChainPresenter(CommandScheduler& cmdScheduler);

    bool Initialize(Device& device, const SwapChainDesc& desc);
    void Clear(CommandList& cmd, float r, float g, float b, float a);
    void TransitionToRenderTarget(CommandList& cmd);
    void SetRenderTarget(CommandList& cmd);
    void SetViewport(CommandList& cmd, const std::optional<Rect>& viewport = std::nullopt);
    void TransitionToPresent(CommandList& cmd);
    bool Present(bool vsync);
    bool Resize(Device& device, const Size& size);
    const Size& GetSize() { return m_size; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;
    Resource& GetCurrentBackbuffer() { return m_renderTargets[m_frameIndex]; }

private:
    bool CreateSwapChain(Device& device, ID3D12CommandQueue* queue, const SwapChainDesc& desc);
    bool CreateRTV(Device& device);
    bool CreateDSV(Device& device);
    bool CreateFrameRTVs(Device& device);
    bool CreateDepthBuffer(Device& device);
    void HandleDeviceLost();

    CommandScheduler& m_cmdScheduler;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    vector<Resource> m_renderTargets;

    Resource m_depthBuffer;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    DXGI_FORMAT m_depthFormat{ DXGI_FORMAT_D32_FLOAT };

    Size m_size{};
    bool m_tearing{ false };
    UINT m_frameIndex = 0;
    UINT m_rtvDescriptorSize = 0;
    UINT m_frameCount{ 2 };
};