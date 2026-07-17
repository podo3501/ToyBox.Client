#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <mutex>
#include "GameClient/Service/Render/RenderConfig.h"

class Device;

using Microsoft::WRL::ComPtr;

class BindlessDescriptorAllocator
{
public:
    ~BindlessDescriptorAllocator();
    BindlessDescriptorAllocator();
    bool Initialize(Device& device, const BindlessDescriptorConfig& config) noexcept;
    UINT Allocate() noexcept; //persistent 앞에서부터 채워나간다. FrameTransient와 공유함.
    UINT AllocateFrameTransient(UINT count = 1) noexcept; // 뒤에서부터 채우는 프레임당 임시 할당
    UINT AllocateAsyncTransient(UINT count = 1) noexcept; // 임시 할당. 프레임 단위가 아니라 fence에 따라 다름. 예약된 고정 크기가 있다.( ex. mipmap 같이 잠시 계산때 쓰고 버리는 거)

    void ResetAll() noexcept;
    void ResetFrameTransient() noexcept; // 임시할당 지우기.
    void ResetAsyncTransient() noexcept;

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const noexcept;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const noexcept;

    ID3D12DescriptorHeap* GetHeap() const { return m_heap.Get(); }
    UINT GetDescriptorSize() const { return m_descriptorSize; }

private:
    ComPtr<ID3D12DescriptorHeap> m_heap;

    UINT m_descriptorSize{ 0 };

    UINT m_allocFront{ 0 }; //앞에서부터 채워지는것. 지워지지 않을것. 스택 느낌
    UINT m_frameAllocBack{ 0 }; //뒤에서부터 채워지는 것. 임시로 만들었다가 지워지는것들. 힙 느낌. 0부터 올라간다. 

    UINT m_asyncTransientStart{ 0 };
    UINT m_asyncTransientAlloc{ 0 };
    UINT m_asyncTransientCount{ 0 };

    std::mutex m_asyncMutex; //공유 가능한 임시 영역이라 mutex가 필요함.

    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart{};
};