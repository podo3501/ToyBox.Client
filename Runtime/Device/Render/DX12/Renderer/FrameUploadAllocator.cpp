#include "pch.h"
#include "FrameUploadAllocator.h"
#include "../d3dx12.h"

bool FrameUploadAllocator::Initialize(ID3D12Device* device, UINT bufferSize)
{
    m_bufferSize = bufferSize;

    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_resource)
    );

    m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped));

    return true;
}

void FrameUploadAllocator::Reset()
{
    m_offset = 0;
}