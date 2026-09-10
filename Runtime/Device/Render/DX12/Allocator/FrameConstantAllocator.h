#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Resource/Resource.h"
#include "Core/Foundation/Align.h"

class Device;

class FrameConstantAllocator
{
public:
    ~FrameConstantAllocator();
    FrameConstantAllocator();
    void Reset();

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
    UINT m_bufferSize{};
};

template<typename T>
void FrameConstantAllocator::Initialize(Device& device, UINT count)
{
    static_assert(std::is_trivially_copyable_v<T>);
    Assert(count > 0);

    m_stride = static_cast<UINT>(Core::AlignUp(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

    CreateBuffer(device, m_stride * count);
}

template<typename T>
D3D12_GPU_VIRTUAL_ADDRESS FrameConstantAllocator::AllocateConstant(const T& data)
{
    static_assert(std::is_trivially_copyable_v<T>);

    // 슬롯을 원자적으로 예약 (예약된 시점의 이전 값을 offset으로 사용)
    UINT offset = m_offset.fetch_add(m_stride, std::memory_order_relaxed);

    Assert(offset + m_stride <= m_bufferSize);
    Assert(sizeof(T) <= m_stride);

    memcpy(m_mapped + offset, &data, sizeof(T));

    return m_resource->GetGPUVirtualAddress() + offset;
}
