#pragma once
#include <cstdint>
#include <d3d12.h>
#include "Resource/Resource.h"

struct UploadAllocation // 할당된 영역의 CPU/GPU 주소를 반환할 구조체
{
    Resource* resource{ nullptr };
    uint8_t* cpuAddress{ nullptr };
    D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ 0 };
    size_t offset{ 0 };

    explicit operator bool() const noexcept
    {
        return cpuAddress != nullptr;
    }
};