#pragma once
#include <d3d12.h>

enum class CommandType
{
    None,
    Direct, //랜더링
    Copy, //리소스 전송
    Compute //계산 
};

inline D3D12_COMMAND_LIST_TYPE ToD3D12(CommandType type)
{
    switch (type)
    {
    case CommandType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
    case CommandType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
    case CommandType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
    default: return D3D12_COMMAND_LIST_TYPE_DIRECT; // 안전 기본값
    }
}
