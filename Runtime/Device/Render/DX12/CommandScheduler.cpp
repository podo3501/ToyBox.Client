#include "pch.h"
#include "CommandScheduler.h"
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler()
{
    if (m_eventHandle)
        CloseHandle(m_eventHandle);
}
CommandScheduler::CommandScheduler(const DX12DeviceView& dv) :
    m_dv{ dv }
{}

bool CommandScheduler::Initialize()
{
    ReturnIfFalse(CreateCommandObjects());
    ReturnIfFalse(CreateFence());
    ReturnIfFalse(CreateCopyFence());

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
    m_dv.queue->ExecuteCommandLists(1, lists);

    return FlushGPU();
}

bool CommandScheduler::FlushGPU()
{
    const UINT64 fenceToWait = m_fenceValue;
    if (FAILED(m_dv.queue->Signal(m_fence.Get(), fenceToWait)))
        return false;

    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fenceToWait)
    {
        if (FAILED(m_fence->SetEventOnCompletion(fenceToWait, m_eventHandle))) return false;
        WaitForSingleObject(m_eventHandle, INFINITE);
    }

    return true;
}

static ComPtr<ID3D12CommandAllocator> CreateUploadAllocator(ID3D12Device* device)
{
    ComPtr<ID3D12CommandAllocator> allocator;
    HRESULT hr = device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_COPY,
        IID_PPV_ARGS(&allocator)
    );
    if (FAILED(hr)) return nullptr;
    return allocator;
}

ID3D12GraphicsCommandList* CommandScheduler::CreateUploadCommandList()
{
    if (FAILED(m_allocatorUpload->Reset())) return nullptr;
    if (FAILED(m_commandUpload->Reset(m_allocatorUpload.Get(), nullptr))) return nullptr;

    return m_commandUpload.Get();
}

bool CommandScheduler::ExecuteUploadCommandList()
{
    if (!m_commandUpload) return false;

    m_commandUpload->Close();

    ID3D12CommandList* lists[] = { m_commandUpload.Get() };
    m_dv.copyQueue->ExecuteCommandLists(1, lists);

    FlushUpdateGPU(); // 업로드 완료 대기

    return true;
}

bool CommandScheduler::FlushUpdateGPU()
{
    const UINT64 fenceToWait = m_copyFenceValue;
    if (FAILED(m_dv.copyQueue->Signal(m_fence.Get(), fenceToWait)))
        return false;

    m_copyFenceValue++;

    if (m_copyFence->GetCompletedValue() < fenceToWait)
    {
        if (FAILED(m_fence->SetEventOnCompletion(fenceToWait, m_copyEventHandle))) return false;
        WaitForSingleObject(m_copyEventHandle, INFINITE);
    }

    return true;
}

bool CommandScheduler::CreateCommandObjects()
{
    if (FAILED(m_dv.device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_allocator))))
        return false;

    if (FAILED(m_dv.device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_allocator.Get(),
        nullptr,
        IID_PPV_ARGS(&m_commandList))))
        return false;
    m_commandList->Close(); // 처음엔 닫아야 함

    if (FAILED(m_dv.device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_COPY,
        IID_PPV_ARGS(&m_allocatorUpload)))) 
        return false;

    if (FAILED(m_dv.device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_COPY,
        m_allocatorUpload.Get(),
        nullptr,
        IID_PPV_ARGS(&m_commandUpload))))
        return false;
    m_commandUpload->Close(); // 처음엔 닫아야 함

    return true;
}

bool CommandScheduler::CreateFence()
{
    if (FAILED(m_dv.device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&m_fence))))
        return false;

    m_fenceValue = 1;

    m_eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_eventHandle) return false;

    return true;
}

bool CommandScheduler::CreateCopyFence()
{
    if (FAILED(m_dv.device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&m_copyFence))))
        return false;

    m_copyFenceValue = 1;

    m_copyEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_copyEventHandle) return false;

    return true;
}

ID3D12GraphicsCommandList* CommandScheduler::GetCommandList()
{
    return m_commandList.Get();
}