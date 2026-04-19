#pragma once
#include "FenceTypes.h"
#include "CommandType.h"
#include <d3d12.h>

class DescriptorAllocator;

class DescriptorAllocation
{
public:
    ~DescriptorAllocation();
    DescriptorAllocation() = default;
    DescriptorAllocation(DescriptorAllocator* allocator, UINT index);

    DescriptorAllocation(const DescriptorAllocation&) = delete;
    DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;
    DescriptorAllocation(DescriptorAllocation&& other) noexcept;
    DescriptorAllocation& operator=(DescriptorAllocation&& other) noexcept;

    bool IsValid() const { return m_index != UINT_MAX; }
    void MarkUsed(const QueueFences& fences);
    void MarkUsed(CommandType type, uint64_t fence);
    
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const;

private:
    void MoveFrom(DescriptorAllocation& other);
    void Release();

private:
    DescriptorAllocator* m_allocator{ nullptr };
    UINT m_index{ UINT_MAX };

    QueueFences m_fences{};
    bool m_deferred{ false };
};