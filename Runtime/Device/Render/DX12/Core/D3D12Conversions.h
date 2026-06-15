#pragma once
#include <d3d12.h>

enum class CommandType;
D3D12_COMMAND_LIST_TYPE ToD3D12(CommandType type);

enum class PrimitiveTopologyType;
D3D_PRIMITIVE_TOPOLOGY ToD3D12(PrimitiveTopologyType topology);