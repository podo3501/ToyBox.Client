#pragma once
#include "Core/Utils/Allocator/IndexAllocator.h"
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
    UINT Allocate() noexcept { return m_indexAllocator.Allocate(); }
    void Free(UINT index) noexcept { m_indexAllocator.Free(index); } // 주의! Pending 되어서 gpu로부터 안전하게 된 것만 지우도록 해야 한다.
    void Reset() noexcept { m_indexAllocator.Reset(); }

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const noexcept;
    ID3D12DescriptorHeap* GetHeap() const noexcept { return m_heap.Get(); }
    UINT GetDescriptorSize() const noexcept { return m_descriptorSize; }

private:
    Core::IndexAllocator m_indexAllocator;
    ComPtr<ID3D12DescriptorHeap> m_heap;

    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{};
    UINT m_descriptorSize{ 0 };
};