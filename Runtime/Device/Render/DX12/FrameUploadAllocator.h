#pragma once
#include <d3d12.h>
#include <wrl.h>

class FrameUploadAllocator
{
public:
    bool Initialize(ID3D12Device* device, UINT bufferSize);
    void Reset();

    template<typename T>
    D3D12_GPU_VIRTUAL_ADDRESS AllocateConstant(const T& data);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;

    static constexpr UINT kAlignment = 256;

    uint8_t* m_mapped{};
    UINT m_offset{};
    UINT m_bufferSize{};
};

template<typename T>
D3D12_GPU_VIRTUAL_ADDRESS FrameUploadAllocator::AllocateConstant(const T& data)
{
    UINT alignedOffset = (m_offset + (kAlignment - 1)) & ~(kAlignment - 1);
    UINT requiredSize = alignedOffset + kAlignment;

    Assert(requiredSize <= m_bufferSize);

    memcpy(m_mapped + alignedOffset, &data, sizeof(T));

    auto gpuAddress = m_resource->GetGPUVirtualAddress() + alignedOffset;
    m_offset = requiredSize;

    return gpuAddress;
}
