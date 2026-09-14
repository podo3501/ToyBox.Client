#pragma once
#include "RenderConstants.h"
#include "UploadAllocation.h"
#include "Resource/Resource.h"
#include "Core/Utils/Allocator/LinearIndexAllocator.h"

class Device;

class FrameUploadAllocator
{
public:
    FrameUploadAllocator();
    ~FrameUploadAllocator();

    bool Initialize(
        Device& device, 
        UINT bufferSizeInBytes,
        UINT elementStride);
    void Reset(uint32_t slot);
    
    UploadAllocation Allocate(UINT elementCount) noexcept;
    bool IsInitialized() const noexcept { return m_perSlotSize > 0; }

private:
    Resource m_resource;
    UINT m_elementStride{ 0 };
    UINT m_perSlotSize{ 0 };
    uint8_t* m_mapped{ nullptr };

    std::array<Core::LinearIndexAllocator, FrameBufferCount> m_region;
    uint32_t m_currentSlot{ 0 };
};