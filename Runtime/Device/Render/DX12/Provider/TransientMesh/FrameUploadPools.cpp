#include "pch.h"
#include "FrameUploadPools.h"
#include "Core/Device.h"

bool FrameUploadPools::Initialize(Device& device, std::span<const FrameUploadPoolDesc> poolDescs)
{
    for (const auto& desc : poolDescs)
    {
        size_t index = static_cast<size_t>(desc.type);
        Assert(index < Core::EnumSize<FrameUploadPoolType>);
        Assert(!m_pools[index].IsInitialized()); // 같은 타입 중복 등록 방지

        if (!m_pools[index].Initialize(device, desc.bufferSizeInBytes, desc.elementStride))
            return false;
    }

    return true;
}

UploadAllocation FrameUploadPools::Allocate(FrameUploadPoolType type, UINT elementCount) noexcept
{
    return Get(type).Allocate(elementCount);
}

void FrameUploadPools::Reset(uint32_t slot) noexcept
{
    for (auto& pool : m_pools)
    {
        if (pool.IsInitialized())
            pool.Reset(slot);
    }
}

FrameUploadAllocator& FrameUploadPools::Get(FrameUploadPoolType type) noexcept
{
    size_t index = static_cast<size_t>(type);
    Assert(index < Core::EnumSize<FrameUploadPoolType>);
    Assert(m_pools[index].IsInitialized()); // 등록 안 된 타입을 실수로 요청하는 걸 여기서 잡음

    return m_pools[index];
}