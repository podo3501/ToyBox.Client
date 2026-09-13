#pragma once
#include "Allocator/FrameUploadAllocator.h"
#include "FrameUploadPoolType.h"

class Device;

struct FrameUploadPoolDesc
{
    FrameUploadPoolType type;
    UINT bufferSizeInBytes;
    UINT elementStride;
};

class FrameUploadPools
{
public:
    FrameUploadPools() = default;
    ~FrameUploadPools() = default;

    FrameUploadPools(const FrameUploadPools&) = delete;
    FrameUploadPools& operator=(const FrameUploadPools&) = delete;

    bool Initialize(Device& device, std::span<const FrameUploadPoolDesc> poolDescs);
    UploadAllocation Allocate(FrameUploadPoolType type, UINT elementCount) noexcept;
    void Reset(uint32_t slot) noexcept;

private:
    FrameUploadAllocator& Get(FrameUploadPoolType type) noexcept;

    std::array<FrameUploadAllocator, Core::EnumSize<FrameUploadPoolType>> m_pools;
};
