#include "pch.h"
#include "ViewTargetResource.h"
#include "Core/Device.h"
#include "Definition/RenderFormat.h"
#include "Graph/RGRenderIDAllocator.h"
#include "Factory/DescriptorFactory.h"
#include "Helpers/TextureHelpers.h"

static Resource CreateColorTarget(Device& device, const Size& size)
{
    auto desc = CreateTextureDescriptor(size.width, size.height, RenderFormat::BackBufferFormat);
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = RenderFormat::BackBufferFormat;
    clearValue.Color[0] = clearValue.Color[1] = clearValue.Color[2] = clearValue.Color[3] = 0.0f; // PMA 컨벤션: 완전 투명 = RGBA 모두 0

    return device.CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue);
}

static Resource CreateDepthTarget(Device& device, const Size& size)
{
    auto desc = CreateTextureDescriptor(size.width, size.height, DXGI_FORMAT_R32_TYPELESS);
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = RenderFormat::DepthFormat;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    return device.CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue);
}

ViewTargetResource::~ViewTargetResource()
{
    if (m_descFactory) // pending 한 다음에 소멸자가 불려야 한다.
    {
        m_descFactory->FreeRTV(m_colorRTVIndex);
        m_descFactory->FreeDSV(m_depthDSVIndex);
    }

    if (m_idAllocator)
    {
        m_idAllocator->FreeDynamic(m_colorID);
        m_idAllocator->FreeDynamic(m_depthID);
    }
}
ViewTargetResource::ViewTargetResource() = default;

bool ViewTargetResource::Initialize(
    Device& device, 
    DescriptorFactory& descFactory, 
    RGRenderIDAllocator& idAllocator,
    const Size& size)
{
    m_descFactory = &descFactory;
    m_idAllocator = &idAllocator;
    m_size = size;

    m_color = CreateColorTarget(device, size);
    m_depth = CreateDepthTarget(device, size);
    if (!m_color || !m_depth) return false;

    m_colorID = idAllocator.AllocateDynamic();
    m_depthID = idAllocator.AllocateDynamic();

    m_colorRTVIndex = descFactory.CreateTextureRTV(m_color, RenderFormat::BackBufferFormat);
    m_depthDSVIndex = descFactory.CreateTextureDSV(m_depth, DXGI_FORMAT_D32_FLOAT);
    m_heapIndex = descFactory.CreateTextureSRV(m_color, RenderFormat::BackBufferFormat);

    bool result = m_colorRTVIndex != UINT_MAX && m_depthDSVIndex != UINT_MAX && m_heapIndex != UINT_MAX;
   
    if (result)
        m_ready = true;

    return result;
}
