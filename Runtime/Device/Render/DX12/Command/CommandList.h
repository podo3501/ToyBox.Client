#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "CommandType.h"
#include "Definition/RenderStateTypes.h"

class Device;

using Microsoft::WRL::ComPtr;

class CommandList
{
public:
    ~CommandList();
    CommandList();

    CommandList(const CommandList&) = delete;
    CommandList& operator=(const CommandList&) = delete;
    CommandList(CommandList&&) noexcept = default;
    CommandList& operator=(CommandList&&) noexcept = default;

    bool Initialize(Device& device, CommandType type);
    void SetBindlessHeap(ID3D12DescriptorHeap* heap);
    void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature);
    void SetPipelineState(
        ID3D12PipelineState* pso,
        std::optional<PrimitiveTopologyType> topology = std::nullopt);
    void Reset();
    void Close();

    bool IsAvailable() const;
    void MarkSubmitted(ID3D12Fence* fence, FenceID fenceID);
    void Discard();

    ID3D12GraphicsCommandList* operator->() const { return m_command.Get(); }
    ID3D12GraphicsCommandList* Get() { return m_command.Get(); }

private:
    enum class CmdState
    {
        Ready,          // 재사용 가능한 초기/유휴 상태
        Recording,      // CPU가 기록 중 (Reset ~ Close 사이)
        PendingSubmit,  // Close됨, 아직 큐에 제출(MarkSubmitted) 안 됨
        InFlight,       // 제출됨, fence로 GPU 완료를 기다리는 중
    };

    CommandType m_type{ CommandType::None };
    mutable CmdState m_state{ CmdState::Ready };

    ComPtr<ID3D12CommandAllocator> m_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_command;

    ID3D12RootSignature* m_currentRootSignature{ nullptr };
    ID3D12PipelineState* m_currentPSO{ nullptr };
    ID3D12Fence* m_fence{ nullptr }; //gpu가 쓰는중

    FenceID m_lastFenceID{ InvalidFenceID }; // 0값은 초기값.
};