#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <mutex>

class Device;

using Microsoft::WRL::ComPtr;

class DescriptorHeapAllocator
{
public:
    ~DescriptorHeapAllocator();
    explicit DescriptorHeapAllocator() noexcept;
    bool Initialize(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT maxCount) noexcept;
    UINT Allocate() noexcept;
    UINT AllocateTransient(UINT count = 1) noexcept; // 뒤에서부터 채우는 임시 할당(mipmap 같이 잠시 계산때 쓰고 버리는 거)

    void Reset() noexcept;
    void ResetTransient() noexcept; // 임시할당 지우기.

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const noexcept;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const noexcept;

    ID3D12DescriptorHeap* GetHeap() const { return m_heap.Get(); }
    UINT GetDescriptorSize() const { return m_descriptorSize; }

private:
    ComPtr<ID3D12DescriptorHeap> m_heap;

    bool m_shaderVisible{ false }; //dsv랑 점점 다른게 많아지면 클래스를 분리해야 함.
    UINT m_descriptorSize{ 0 };
    UINT m_capacity{ 0 };
    UINT m_allocFront{ 0 }; //앞에서부터 채워지는것. 지워지지 않을것. 스택 느낌
    UINT m_allocBack{ 0 }; //뒤에서부터 채워지는 것. 임시로 만들었다가 지워지는것들. 힙 느낌.
    std::mutex m_allocBackMutex; //뒤에서 부터 채워지는 건 공유 가능한 임시 영역이라 mutex가 필요함.

    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart{};
};