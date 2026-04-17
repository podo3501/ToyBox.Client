#pragma once
#include <d3d12.h>

enum class CommandType
{
    Direct, //랜더링
    Copy //리소스 전송
};

inline D3D12_COMMAND_LIST_TYPE ToD3D12(CommandType type)
{
    switch (type)
    {
    case CommandType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
    case CommandType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
    default: return D3D12_COMMAND_LIST_TYPE_DIRECT; // 안전 기본값
    }
}
