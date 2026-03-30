#include "pch.h"
#include "CommandScheduler.h"
#include "d3dx12.h"

CommandScheduler::~CommandScheduler()
{
    if (m_eventHandle)
        CloseHandle(m_eventHandle);
}
CommandScheduler::CommandScheduler(DX12Core core) :
    m_core(core)
{}

bool CommandScheduler::Initialize()
{
    ReturnIfFalse(CreateCommandObjects());
    ReturnIfFalse(CreateFence());

    return true;
}

bool CommandScheduler::BeginFrame()
{
    if (FAILED(m_allocator->Reset())) return false;
    if (FAILED(m_commandList->Reset(m_allocator.Get(), nullptr))) return false;

    return true;
}

bool CommandScheduler::EndFrame()
{
    if (FAILED(m_commandList->Close())) return false;

    ID3D12CommandList* lists[] = { m_commandList.Get() };
    m_core.queue->ExecuteCommandLists(1, lists);

    return FlushGPU();
}

bool CommandScheduler::FlushGPU()
{
    const UINT64 fenceToWait = m_fenceValue;
    if (FAILED(m_core.queue->Signal(m_fence.Get(), fenceToWait)))
        return false;

    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fenceToWait)
    {
        if (FAILED(m_fence->SetEventOnCompletion(fenceToWait, m_eventHandle))) return false;
        WaitForSingleObject(m_eventHandle, INFINITE);
    }

    return true;
}

bool CommandScheduler::CreateCommandObjects()
{
    if (FAILED(m_core.device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_allocator))))
        return false;

    if (FAILED(m_core.device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_allocator.Get(),
        nullptr,
        IID_PPV_ARGS(&m_commandList))))
        return false;

    m_commandList->Close(); // 처음엔 닫아야 함

    return true;
}

bool CommandScheduler::CreateFence()
{
    if (FAILED(m_core.device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&m_fence))))
        return false;

    m_fenceValue = 1;

    m_eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_eventHandle) return false;

    return true;
}

ID3D12GraphicsCommandList* CommandScheduler::GetCommandList()
{
    return m_commandList.Get();
}