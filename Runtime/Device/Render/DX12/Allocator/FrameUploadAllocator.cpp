#include "pch.h"
#include "FrameUploadAllocator.h"
#include "Core/Device.h"
#include "Core/Foundation/Align.h"

FrameUploadAllocator::FrameUploadAllocator() = default;
FrameUploadAllocator::~FrameUploadAllocator() = default;

bool FrameUploadAllocator::Initialize(Device& device, UINT bufferSizeInBytes)
{
    Assert(bufferSizeInBytes > 0);
    m_perSlotSize = bufferSizeInBytes;

    m_resource = device.CreateResource(
        CD3DX12_RESOURCE_DESC::Buffer(bufferSizeInBytes * FrameBufferCount), // 슬롯 수만큼 버퍼를 늘려서, 슬롯마다 겹치지 않는 영역을 갖게 한다.
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    auto hr = m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped));
    Assert(SUCCEEDED(hr));

    return true;
}

UploadAllocation FrameUploadAllocator::Allocate(UINT sizeInBytes, UINT alignment)
{
    UINT alignedOffset = Core::AlignUp(m_offset, alignment);
    Assert(alignedOffset + sizeInBytes <= m_perSlotSize);

    m_offset = alignedOffset + sizeInBytes;

    UINT globalOffset = m_slotBaseOffset + alignedOffset;

    UploadAllocation alloc;
    alloc.resource = &m_resource;
    alloc.cpuAddress = m_mapped + globalOffset;
    alloc.gpuAddress = m_resource->GetGPUVirtualAddress() + globalOffset;
    alloc.offset = globalOffset;

    return alloc;
}

void FrameUploadAllocator::Reset(uint32_t slot)
{
    Assert(slot < FrameBufferCount);

    m_slotBaseOffset = m_perSlotSize * slot;
    m_offset = 0;
}