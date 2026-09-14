#include "pch.h"
#include "FrameUploadAllocator.h"
#include "Core/Device.h"
#include "Core/Foundation/Align.h"

FrameUploadAllocator::FrameUploadAllocator() = default;
FrameUploadAllocator::~FrameUploadAllocator() = default;

bool FrameUploadAllocator::Initialize(
    Device& device, 
    UINT bufferSizeInBytes, 
    UINT elementStride)
{
    Assert(bufferSizeInBytes > 0);
    Assert(elementStride > 0);

    m_elementStride = elementStride;
    m_perSlotSize = Core::AlignUpGeneric(bufferSizeInBytes, elementStride);

    UINT slotCapacityInElements = m_perSlotSize / elementStride;
    for (auto& region : m_region)
        region.Initialize(slotCapacityInElements);

    m_resource = device.CreateResource(
        CD3DX12_RESOURCE_DESC::Buffer(m_perSlotSize * FrameBufferCount), // 슬롯 수만큼 버퍼를 늘려서, 슬롯마다 겹치지 않는 영역을 갖게 한다.
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    auto hr = m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped));
    Assert(SUCCEEDED(hr));

    return true;
}

UploadAllocation FrameUploadAllocator::Allocate(UINT elementCount) noexcept
{
    Assert(m_elementStride > 0); // 등록 없이 Allocate 호출 금지

    UINT localElement = m_region[m_currentSlot].Allocate(elementCount);
    Assert(localElement != Core::InvalidIndex); // capacity 초과 - bufferSizeInBytes를 늘려야 함

    UINT sizeInBytes = elementCount * m_elementStride;
    UINT globalOffset = m_perSlotSize * m_currentSlot + localElement * m_elementStride; // slot offset + element offset

    UploadAllocation alloc;
    alloc.resource = &m_resource;
    alloc.cpuAddress = m_mapped + globalOffset;
    alloc.gpuAddress = m_resource->GetGPUVirtualAddress() + globalOffset;
    alloc.offset = globalOffset;
    alloc.sizeInBytes = sizeInBytes;

    return alloc;
}

void FrameUploadAllocator::Reset(uint32_t slot)
{
    Assert(slot < FrameBufferCount);

    m_currentSlot = slot;
    m_region[slot].Reset();
}