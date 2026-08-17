#include "pch.h"
#include "D3D12Conversions.h"
#include "Command/CommandType.h"
#include "Graph/RGTypes.h"
#include "GameClient/Service/Render/Definition/View/RenderState.h"
#include "GameClient/Asset/TextureAsset.h"

// ToD3D12

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

D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12_PSO(PrimitiveTopologyType topology)
{
    switch (topology)
    {
    case PrimitiveTopologyType::Triangle: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case PrimitiveTopologyType::Line: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    default: Assert(false); return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }
}

D3D_PRIMITIVE_TOPOLOGY ToD3D12_Draw(PrimitiveTopologyType topology)
{
    switch (topology)
    {
    case PrimitiveTopologyType::Triangle: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveTopologyType::Line: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    default: Assert(false); return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}

D3D12_RESOURCE_STATES ToD3D12(RGAccess access)
{
    switch (access)
    {
    case RGAccess::SRV: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    case RGAccess::UAV: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    //case RGAccess::CopySrc: return D3D12_RESOURCE_STATE_COPY_SOURCE;
    //case RGAccess::CopyDst: return D3D12_RESOURCE_STATE_COPY_DEST;
    case RGAccess::RTV: return D3D12_RESOURCE_STATE_RENDER_TARGET;
    case RGAccess::DepthWrite: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
    case RGAccess::DepthRead: return D3D12_RESOURCE_STATE_DEPTH_READ;
    case RGAccess::Present: return D3D12_RESOURCE_STATE_PRESENT;
    default: return D3D12_RESOURCE_STATE_COMMON;
    }
}

D3D12_FILL_MODE ToD3D12(FillMode mode)
{
    switch (mode)
    {
    case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
    case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
    default: Assert(false); return D3D12_FILL_MODE_SOLID;
    }
}

D3D12_CULL_MODE ToD3D12(CullMode mode)
{
    switch (mode)
    {
    case CullMode::None:  return D3D12_CULL_MODE_NONE;
    case CullMode::Front: return D3D12_CULL_MODE_FRONT;
    case CullMode::Back:  return D3D12_CULL_MODE_BACK;
    default: Assert(false); return D3D12_CULL_MODE_NONE;
    }
}

// ToDXGI

DXGI_FORMAT ToSRGB(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case DXGI_FORMAT_BC1_UNORM: return DXGI_FORMAT_BC1_UNORM_SRGB;
    case DXGI_FORMAT_BC2_UNORM: return DXGI_FORMAT_BC2_UNORM_SRGB;
    case DXGI_FORMAT_BC3_UNORM: return DXGI_FORMAT_BC3_UNORM_SRGB;
    default:
        return format; // 이미 SRGB거나 변환 불가
    }
}

DXGI_FORMAT ToDXGIFormat(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
    case PixelFormat::RGB8: return DXGI_FORMAT_R8G8B8A8_UNORM; //3채널은 지원하지 않는다. 일단 이걸로.
    case PixelFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case PixelFormat::R11G11B10F: return DXGI_FORMAT_R11G11B10_FLOAT;
    case PixelFormat::RGBA16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case PixelFormat::RGB9E5: return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
    case PixelFormat::BC6H_UF16: return DXGI_FORMAT_BC6H_UF16;
    }

    return DXGI_FORMAT_R8G8B8A8_UNORM;
}