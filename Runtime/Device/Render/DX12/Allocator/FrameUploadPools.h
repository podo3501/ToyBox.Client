#pragma once
#include "Allocator/FrameUploadAllocator.h"

class Device;

class FrameUploadPools
{
public:
    FrameUploadPools() = default;
    ~FrameUploadPools() = default;

    FrameUploadPools(const FrameUploadPools&) = delete;
    FrameUploadPools& operator=(const FrameUploadPools&) = delete;

    bool Initialize(Device& device);
    void Reset() noexcept;

    FrameUploadAllocator& VertexBuffer() noexcept;
    FrameUploadAllocator& IndexBuffer() noexcept;

private:
    static constexpr UINT VertexBufferSize = 16 * 1024 * 1024; //16MB
    static constexpr UINT IndexBufferSize = 16 * 1024 * 1024;

    FrameUploadAllocator m_vertexBuffer;
    FrameUploadAllocator m_indexBuffer;
};
