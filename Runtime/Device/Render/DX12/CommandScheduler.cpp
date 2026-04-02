#include "pch.h"
#include "CommandScheduler.h"
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler() = default;
CommandScheduler::CommandScheduler() = default;

bool CommandScheduler::Initialize(ID3D12Device* device)
{
    ReturnIfFalse(m_direct.Initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT));
    ReturnIfFalse(m_copy.Initialize(device, D3D12_COMMAND_LIST_TYPE_COPY));

    return true;
}

ID3D12GraphicsCommandList* CommandScheduler::Begin(CommandType type)
{
    if (m_currentContext) return nullptr;

    CommandContext* cmd = GetCommandContext(type);
    if (!cmd) return nullptr;

    if (!cmd->Reset()) return nullptr;
    m_currentContext = cmd;

    return cmd->Get();
}

bool CommandScheduler::End()
{
    if (!m_currentContext) return false;

    ReturnIfFalse(m_currentContext->Close());
    ReturnIfFalse(m_currentContext->Flush());
    m_currentContext = nullptr;

    return true;
}

bool CommandScheduler::Flush()
{
    ReturnIfFalse(m_direct.Flush());
    ReturnIfFalse(m_copy.Flush());
    return true;
}

CommandContext* CommandScheduler::GetCommandContext(CommandType type)
{
    switch (type)
    {
    case CommandType::Direct: return &m_direct;
    case CommandType::Copy:   return &m_copy;
    }
    return nullptr;
}

ID3D12CommandQueue* CommandScheduler::GetCommandQueue(CommandType type)
{
    switch (type)
    {
    case CommandType::Direct: return m_direct.GetQueue();
    case CommandType::Copy:   return m_copy.GetQueue();
    }
    return nullptr;
}

ID3D12GraphicsCommandList* CommandScheduler::GetCommandList(CommandType type)
{
    switch (type)
    {
    case CommandType::Direct: return m_direct.Get();
    case CommandType::Copy:   return m_copy.Get();
    }
    return nullptr;
}

ID3D12GraphicsCommandList* CommandScheduler::GetCurrentCommandList()
{
    if (!m_currentContext) return nullptr;
    return m_currentContext->Get();
}