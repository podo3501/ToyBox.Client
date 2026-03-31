#pragma once
#include <wrl/client.h>
#include "DX12DeviceView.h"
#include "Core/Foundation/Geometry2D.h"

struct IDXGISwapChain4;
struct ID3D12DescriptorHeap;
struct ID3D12GraphicsCommandList;
struct ID3D12Resource;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct Size;
class CommandScheduler;
enum D3D12_RESOURCE_STATES;

class SwapChainPresenter
{
public:
    ~SwapChainPresenter();
    SwapChainPresenter(const DX12DeviceView& dv);

    bool Initialize(HWND hwnd, const Size& size, bool allowTearing, UINT frameCount = 2 );
    void TransitionToRenderTarget(ID3D12GraphicsCommandList* cmdList);
    void SetRenderTarget(ID3D12GraphicsCommandList* cmdList);
    void TransitionToPresent(ID3D12GraphicsCommandList* cmdList);
    bool Present(bool vsync);
    bool Resize(CommandScheduler* cmd, const Size& size);

    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;

private:
    bool CreateSwapChain(HWND hwnd, const Size& size, bool allowTearing);
    bool CreateRTV();
    bool CreateFrameRTVs();

    DX12DeviceView m_dv{};
    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_renderTargets;

    Size m_size{};
    bool m_tearing{ false };
    UINT m_frameIndex = 0;
    UINT m_rtvDescriptorSize = 0;
    UINT m_frameCount{ 2 };
};