#include "pch.h"
#include "FrameUploadAllocator.h"
#include "Core/Device.h"
#include "../d3dx12.h"

FrameUploadAllocator::~FrameUploadAllocator() = default;
FrameUploadAllocator::FrameUploadAllocator() = default;

void FrameUploadAllocator::CreateBuffer(Device& device, UINT bufferSize)
{
    Assert(bufferSize > 0);
    m_bufferSize = bufferSize;

    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    auto hr = device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_resource.GetAddressOf())
    );
    Assert(SUCCEEDED(hr));

    hr = m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped));
    Assert(SUCCEEDED(hr));
}

void FrameUploadAllocator::Reset()
{
    m_offset = 0;
}