#pragma once
#include <d3d12.h>

// ToD3D12

enum class CommandType;
D3D12_COMMAND_LIST_TYPE ToD3D12(CommandType type);

enum class PrimitiveTopologyType;
D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12_PSO(PrimitiveTopologyType topology);
D3D_PRIMITIVE_TOPOLOGY ToD3D12_Draw(PrimitiveTopologyType topology);

enum class RGAccess;
D3D12_RESOURCE_STATES ToD3D12(RGAccess access);

enum class FillMode;
D3D12_FILL_MODE ToD3D12(FillMode mode);

enum class CullMode;
D3D12_CULL_MODE ToD3D12(CullMode mode);

// ToDXGI

DXGI_FORMAT ToSRGB(DXGI_FORMAT format);

enum class PixelFormat;
DXGI_FORMAT ToDXGIFormat(PixelFormat format);