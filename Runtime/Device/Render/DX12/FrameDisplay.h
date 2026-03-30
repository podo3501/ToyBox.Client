#pragma once
#include <wrl/client.h>
#include "DX12Core.h"
#include "Core/Foundation/Geometry2D.h"

struct IDXGISwapChain4;
struct ID3D12DescriptorHeap;
struct ID3D12GraphicsCommandList;
struct ID3D12Resource;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct Size;
class CommandScheduler;

class FrameDisplay
{
public:
    ~FrameDisplay();
    FrameDisplay(DX12Core core);

    bool Initialize(HWND hwnd, const Size& size, bool allowTearing);
    void BindCurrentRTV(ID3D12GraphicsCommandList* cmdList);
    bool Present(bool vsync);
    bool Resize(CommandScheduler* cmd, const Size& size);

    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;

private:
    bool CreateSwapChain(HWND hwnd, const Size& size, bool allowTearing);
    bool CreateRTV();
    bool CreateFrameRTVs();

    constexpr static UINT FrameCount = 2;

    DX12Core m_core;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];

    Size m_size{};
    bool m_tearing{ false };
    UINT m_frameIndex = 0;
    UINT m_rtvDescriptorSize = 0;
};