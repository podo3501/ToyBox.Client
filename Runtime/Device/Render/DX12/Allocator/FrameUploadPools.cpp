#include "pch.h"
#include "FrameUploadPools.h"
#include "Core/Device.h"

bool FrameUploadPools::Initialize(Device& device)
{
    ReturnIfFalse(m_vertexBuffer.Initialize(device, VertexBufferSize));
    ReturnIfFalse(m_indexBuffer.Initialize(device, IndexBufferSize));

    return true;
}

void FrameUploadPools::Reset(uint32_t slot) noexcept
{
    m_vertexBuffer.Reset(slot);
    m_indexBuffer.Reset(slot);
}

FrameUploadAllocator& FrameUploadPools::VertexBuffer() noexcept { return m_vertexBuffer; }
FrameUploadAllocator& FrameUploadPools::IndexBuffer() noexcept { return m_indexBuffer; }