#pragma once
#include "RenderConstants.h"
#include "UploadAllocation.h"
#include "Resource/Resource.h"

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
    
    //UploadAllocation Allocate(UINT sizeInBytes, UINT alignment); // 임의의 바이트 크기와 정렬 크기를 인자로 받아 원하는 만큼 잘라주는 함수
    UploadAllocation Allocate(UINT elementCount) noexcept;
    bool IsInitialized() const noexcept { return m_perSlotSize > 0; }

private:
    Resource m_resource;
    UINT m_elementStride{ 0 };
    UINT m_perSlotSize{ 0 };
    uint8_t* m_mapped{ nullptr };

    UINT m_slotBaseOffset{ 0 };
    UINT m_offset{ 0 };
};