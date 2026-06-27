#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Resource/Resource.h"
#include "Helpers/CommonHelpers.h"

class Device;

class FrameUploadAllocator
{
public:
    ~FrameUploadAllocator();
    FrameUploadAllocator();
    void Reset();

    template<typename T>
    void Initialize(Device& device, UINT count);
    template<typename T>
    D3D12_GPU_VIRTUAL_ADDRESS AllocateConstant(const T& data);

private:
    void CreateBuffer(Device& device, UINT bufferSize);

    Resource m_resource;
    uint8_t* m_mapped{};
    UINT m_offset{};
    UINT m_stride{};
    UINT m_bufferSize{};
};

template<typename T>
void FrameUploadAllocator::Initialize(Device& device, UINT count)
{
    static_assert(std::is_trivially_copyable_v<T>);
    Assert(count > 0);

    m_stride = static_cast<UINT>(AlignSize(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

    CreateBuffer(device, m_stride * count);
}

template<typename T>
D3D12_GPU_VIRTUAL_ADDRESS FrameUploadAllocator::AllocateConstant(const T& data)
{
    static_assert(std::is_trivially_copyable_v<T>);

    Assert(m_offset + m_stride <= m_bufferSize);
    Assert(sizeof(T) <= m_stride);

    memcpy(m_mapped + m_offset, &data, sizeof(T));

    auto gpuAddress = m_resource->GetGPUVirtualAddress() + m_offset;
    m_offset += m_stride;

    return gpuAddress;
}
