#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "DescriptorAllocation.h"

using Microsoft::WRL::ComPtr;

struct SubmittedFences;
struct CompletedFences;
struct PendingFree;

class DescriptorAllocator
{
public:
    ~DescriptorAllocator();
    explicit DescriptorAllocator(ID3D12Device* device) noexcept;
    bool Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT maxCount, bool shaderVisible) noexcept;
    DescriptorAllocation Allocate() noexcept;
    void Free(UINT index); //일반적으로 잘 쓰지 않는다.
    void DeferredFree(UINT index, const QueueFences& fences);
    void ProcessDeferredFree(const QueueFences& fences); //매프레임당 부르는 함수

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const noexcept;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const noexcept;

    ID3D12DescriptorHeap* GetHeap() const { return m_heap.Get(); }
    UINT GetDescriptorSize() const { return m_descriptorSize; }

private:
    ID3D12Device* m_device{ nullptr };
    ComPtr<ID3D12DescriptorHeap> m_heap;

    UINT m_descriptorSize{ 0 };
    UINT m_capacity{ 0 };
    UINT m_allocated{ 0 }; //현재할당
    bool m_shaderVisible{ false };
    std::vector<UINT> m_freeList;
    vector<PendingFree> m_pendingFrees;

    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart{};
};