#include "pch.h"
#include "FrameUploadAllocator.h"
#include "Core/Device.h"
#include "Core/Foundation/Align.h"

FrameUploadAllocator::FrameUploadAllocator() = default;
FrameUploadAllocator::~FrameUploadAllocator() = default;

bool FrameUploadAllocator::Initialize(Device& device, UINT bufferSizeInBytes)
{
    Assert(bufferSizeInBytes > 0);
    m_bufferSize = bufferSizeInBytes;

    m_resource = device.CreateResource(
        CD3DX12_RESOURCE_DESC::Buffer(bufferSizeInBytes),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    auto hr = m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped));
    Assert(SUCCEEDED(hr));

    return true;
}

UploadAllocation FrameUploadAllocator::Allocate(UINT sizeInBytes, UINT alignment)
{
    UINT alignedOffset = Core::AlignUp(m_offset, alignment);
    Assert(alignedOffset + sizeInBytes <= m_bufferSize);

    m_offset = alignedOffset + sizeInBytes;

    UploadAllocation alloc;
    alloc.resource = &m_resource;
    alloc.cpuAddress = m_mapped + alignedOffset;
    alloc.gpuAddress = m_resource->GetGPUVirtualAddress() + alignedOffset;
    alloc.offset = alignedOffset;

    return alloc;
}

void FrameUploadAllocator::Reset()
{
    m_offset = 0;
}