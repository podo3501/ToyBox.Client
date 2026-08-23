#pragma once
#include <d3d12.h>
#include <wrl/client.h>

class Device;

using Microsoft::WRL::ComPtr;

class DescriptorAllocator
{
public:
    ~DescriptorAllocator();
    DescriptorAllocator();

    bool Initialize(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT capacity) noexcept;
    UINT Allocate() noexcept;
    void Free(UINT index) noexcept; // 주의! Pending 되어서 gpu로부터 안전하게 된 것만 지우도록 해야 한다.
    void Reset() noexcept;

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const noexcept;
    ID3D12DescriptorHeap* GetHeap() const noexcept { return m_heap.Get(); }
    UINT GetDescriptorSize() const noexcept { return m_descriptorSize; }

private:
    ComPtr<ID3D12DescriptorHeap> m_heap;

    UINT m_capacity{ 0 };
    UINT m_allocated{ 0 };
    UINT m_descriptorSize{ 0 };
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{};

    std::vector<UINT> m_freeList; // 해제된 인덱스 보관
};