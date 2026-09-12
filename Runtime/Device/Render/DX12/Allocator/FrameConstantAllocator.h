#pragma once
#include "../RenderConstants.h"
#include "Resource/Resource.h"
#include "Core/Foundation/Align.h"
#include <d3d12.h>
#include <wrl.h>

class Device;

class FrameConstantAllocator
{
public:
    ~FrameConstantAllocator();
    FrameConstantAllocator();
    void Reset(uint32_t slot);

    template<typename T>
    void Initialize(Device& device, UINT count);
    template<typename T>
    D3D12_GPU_VIRTUAL_ADDRESS AllocateConstant(const T& data);

private:
    void CreateBuffer(Device& device, UINT bufferSize);

    Resource m_resource;
    uint8_t* m_mapped{};
    std::atomic<UINT> m_offset{};
    UINT m_stride{};
    UINT m_perSlotSize{};
    UINT m_slotBaseOffset{};
};

template<typename T>
void FrameConstantAllocator::Initialize(Device& device, UINT count)
{
    static_assert(std::is_trivially_copyable_v<T>);
    Assert(count > 0);

    m_stride = static_cast<UINT>(Core::AlignUp(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
    m_perSlotSize = m_stride * count;

    CreateBuffer(device, m_perSlotSize * FrameBufferCount);
}

template<typename T>
D3D12_GPU_VIRTUAL_ADDRESS FrameConstantAllocator::AllocateConstant(const T& data)
{
    static_assert(std::is_trivially_copyable_v<T>);

    UINT localOffset = m_offset.fetch_add(m_stride, std::memory_order_relaxed);
    UINT offset = m_slotBaseOffset + localOffset;

    Assert(localOffset + m_stride <= m_perSlotSize);
    Assert(sizeof(T) <= m_stride);

    memcpy(m_mapped + offset, &data, sizeof(T));

    return m_resource->GetGPUVirtualAddress() + offset;
}
