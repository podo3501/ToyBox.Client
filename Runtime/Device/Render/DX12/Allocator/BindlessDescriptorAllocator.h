#pragma once
#include "Core/Utils/Allocator/DualIndexAllocator.h"
#include "Core/Utils/Allocator/IndexAllocator.h"
#include "GameClient/Service/Render/RenderConfig.h"
#include <d3d12.h>
#include <wrl/client.h>

class Device;

using Microsoft::WRL::ComPtr;

class BindlessDescriptorAllocator
{
public:
    ~BindlessDescriptorAllocator();
    BindlessDescriptorAllocator();
    bool Initialize(Device& device, const BindlessDescriptorConfig& config) noexcept;

    UINT Allocate() noexcept; //persistent 앞에서부터 채워나간다. FrameTransient와 공유함.
    UINT AllocateFrameTransient(UINT count = 1) noexcept; // 뒤에서부터 채우는 프레임당(프레임 끝나면 Free) 임시 할당

    UINT AllocateAsyncTransient() noexcept; // 임시 할당. 프레임 단위가 아니라 fence에 따라 다름. 예약된 고정 크기가 있다.( ex. mipmap 같이 잠시 계산때 쓰고 버리는 거)
    void FreeAsyncTransient(UINT index) noexcept;

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

    Core::DualIndexAllocator m_persistentRegion;   // [0, asyncTransientStart), front=persistent, back=frameTransient
    Core::IndexAllocator m_asyncRegion;      // [asyncTransientStart, +asyncTransientCount), 로컬 인덱스, free 가능
    UINT m_asyncTransientStart{ 0 };         // 로컬 -> 전역 heap 인덱스 변환용 오프셋

    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart{};
};