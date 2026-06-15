#include "pch.h"
#include "D3D12Conversions.h"
#include "Command/CommandType.h"
#include "GameClient/Service/Render/Desc/RenderState.h"

D3D12_COMMAND_LIST_TYPE ToD3D12(CommandType type)
{
    switch (type)
    {
    case CommandType::Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
    case CommandType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
    case CommandType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
    default: return D3D12_COMMAND_LIST_TYPE_DIRECT; // 안전 기본값
    }
}

D3D_PRIMITIVE_TOPOLOGY ToD3D12(PrimitiveTopologyType topology)
{
    switch (topology)
    {
    case PrimitiveTopologyType::Triangle: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveTopologyType::Line: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    default: Assert(false); return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}