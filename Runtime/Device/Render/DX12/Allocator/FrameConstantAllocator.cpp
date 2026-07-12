#include "pch.h"
#include "FrameConstantAllocator.h"
#include "Core/Device.h"
#include "d3dx12.h"

FrameConstantAllocator::~FrameConstantAllocator() = default;
FrameConstantAllocator::FrameConstantAllocator() = default;

void FrameConstantAllocator::CreateBuffer(Device& device, UINT bufferSize)
{
    Assert(bufferSize > 0);
    m_bufferSize = bufferSize;

    m_resource = device.CreateResource(
        CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    auto hr = m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped));
    Assert(SUCCEEDED(hr));
}

void FrameConstantAllocator::Reset()
{
    m_offset = 0;
}