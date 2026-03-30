#pragma once
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "Core/Foundation/Geometry2D.h"
#include "d3d12.h"

constexpr static const UINT FrameCount = 2;

struct Vertex
{
	float x, y, z;
	float r, g, b, a;
};

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

	// Quad
	void BindQuadPipeline(ID3D12GraphicsCommandList* cmd);
	void DrawQuad(ID3D12GraphicsCommandList* cmd);

	ID3D12Device* GetDevice() const;
	ID3D12CommandQueue* GetCommandQueue() const;
	IDXGISwapChain4* GetSwapChain() const;
	ID3D12GraphicsCommandList* GetCommandList() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;

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

	void CreateQuadResources();

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

	// quad
	Microsoft::WRL::ComPtr<ID3D12Resource> m_quadVB;
	D3D12_VERTEX_BUFFER_VIEW m_quadVBView{};

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;
};