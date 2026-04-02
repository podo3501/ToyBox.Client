#pragma once
#include <wrl/client.h>
#include "DX12DeviceView.h"
#include "CommandContext.h"

enum class CommandType
{
    Direct, //랜더링
    Copy //리소스 전송
};

class CommandScheduler
{
public:
    ~CommandScheduler();
    CommandScheduler();
    bool Initialize(ID3D12Device* device);
    ID3D12GraphicsCommandList* Begin(CommandType type);
    bool End();
    bool Flush();

    ID3D12CommandQueue* GetCommandQueue(CommandType type);
    ID3D12GraphicsCommandList* GetCommandList(CommandType type);
    ID3D12GraphicsCommandList* GetCurrentCommandList();

private:
    CommandContext* GetCommandContext(CommandType type);

    CommandContext m_direct;
    CommandContext m_copy;
    CommandContext* m_currentContext{ nullptr };
};
