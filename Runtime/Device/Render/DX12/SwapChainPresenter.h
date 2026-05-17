#pragma once
#include <wrl/client.h>
#include "Core/Foundation/Geometry2D.h"

struct ID3D12Fence;
struct ID3D12Device;
struct IDXGIFactory4;
struct IDXGISwapChain4;
struct ID3D12DescriptorHeap;
struct ID3D12CommandQueue;
struct ID3D12Resource;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct Size;
class CommandList;
class CommandScheduler;
enum D3D12_RESOURCE_STATES;

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
    SwapChainPresenter();

    bool Initialize(ID3D12Device* device, IDXGIFactory4* factory, CommandScheduler* scheduler, const SwapChainDesc& desc);
    void Clear(CommandList& cmd, float r, float g, float b, float a);
    void TransitionToRenderTarget(CommandList& cmd);
    void SetRenderTarget(CommandList& cmd);
    void TransitionToPresent(CommandList& cmd);
    bool Present(bool vsync);
    bool Resize(ID3D12Device* device, const Size& size);
    const Size& GetSize() { return m_size; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;
    ID3D12Resource* GetCurrentBackbuffer() { return m_renderTargets[m_frameIndex].Get(); }

private:
    bool CreateSwapChain(ID3D12Device* device, IDXGIFactory4* factory,
        ID3D12CommandQueue* queue, const SwapChainDesc& desc);
    bool CreateRTV(ID3D12Device* device);
    bool CreateFrameRTVs(ID3D12Device* device);
    bool CreateDepthBuffer(ID3D12Device* device);

    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_renderTargets;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_depthBuffer;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    DXGI_FORMAT m_depthFormat{ DXGI_FORMAT_D32_FLOAT };

    CommandScheduler* m_scheduler{ nullptr };

    Size m_size{};
    bool m_tearing{ false };
    UINT m_frameIndex = 0;
    UINT m_rtvDescriptorSize = 0;
    UINT m_frameCount{ 2 };
};