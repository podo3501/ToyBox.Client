#pragma once
#include "UploadAllocation.h"
#include "Resource/Resource.h"

class Device;

class FrameUploadAllocator
{
public:
    FrameUploadAllocator();
    ~FrameUploadAllocator();

    bool Initialize(Device& device, UINT bufferSizeInBytes);
    void Reset();
    
    UploadAllocation Allocate(UINT sizeInBytes, UINT alignment); // 임의의 바이트 크기와 정렬 크기를 인자로 받아 원하는 만큼 잘라주는 함수

private:
    Resource m_resource;
    uint8_t* m_mapped{};
    UINT m_offset{};
    UINT m_bufferSize{};
};