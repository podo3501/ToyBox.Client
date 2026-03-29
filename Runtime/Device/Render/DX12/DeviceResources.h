#pragma once
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "Core/Foundation/Geometry2D.h"

constexpr static const UINT FrameCount = 2;

struct ID3D12Device;
struct ID3D12CommandQueue;
struct IDXGISwapChain4;
struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;
struct RenderConfig;

struct RenderTargetData
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
	Microsoft::WRL::ComPtr<ID3D12Resource> buffers[FrameCount];
	UINT descriptorSize = 0;
};

struct CommandData
{
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> list;
};

struct SyncData
{
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 value = 0;
	HANDLE eventHandle = nullptr;
};

class DeviceResources
{
public:
	~DeviceResources();
	DeviceResources();

	bool Initialize(HWND hwnd, const Size& size, const RenderConfig& config);
	bool BeginFrame();
	bool EndFrame();
	bool Present(bool vsync);
	bool Resize(const Size& size);

	ID3D12Device* GetDevice() const;
	ID3D12CommandQueue* GetCommandQueue() const;
	IDXGISwapChain4* GetSwapChain() const;

private:
	void CheckTearingSupport(bool& allowTearing);
	bool CreateFactory(bool enableDebug);
	bool CreateDevice();
	bool CreateCommandQueue();
	bool CreateSwapChain(HWND hwnd, const Size& size, bool allowTearing);
	bool CreateRTV();
	bool CreateCommandObjects();
	bool CreateFence();
	bool FlushGPU();

	bool m_tearing{ false };
	Size m_size{};

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

	UINT m_frameIndex = 0;
	RenderTargetData m_rtv;
	CommandData m_command;
	SyncData m_sync;
};