#pragma once
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DescriptorAllocator
{
public:
    ~DescriptorAllocator();
    explicit DescriptorAllocator(ID3D12Device* device) noexcept;
    bool Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT maxCount, bool shaderVisible) noexcept;
    UINT Allocate() noexcept;

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const noexcept;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const noexcept;

    ID3D12DescriptorHeap* GetHeap() const { return m_heap.Get(); }
    UINT GetDescriptorSize() const { return m_descriptorSize; }

private:
    ID3D12Device* m_device{ nullptr };
    ComPtr<ID3D12DescriptorHeap> m_heap;

    UINT m_descriptorSize{ 0 };
    UINT m_capacity{ 0 };
    UINT m_allocated{ 0 }; //현재할당
    bool m_shaderVisible{ false };

    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart{};
};