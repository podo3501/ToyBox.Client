#include "pch.h"
#include "ViewTargetResource.h"
#include "Core/RenderFormat.h"
#include "Core/Device.h"
#include "Graph/RenderGraph.h"
#include "Factory/DescriptorFactory.h"
#include "Helpers/TextureHelpers.h"

ViewTargetResource::~ViewTargetResource() = default;
ViewTargetResource::ViewTargetResource(Device& device, DescriptorFactory& descFactory) :
	m_device{ device },
	m_descFactory{ descFactory } 
{}

ViewTarget& ViewTargetResource::Acquire(uint32_t id, const Size& requiredSize)
{
    auto it = m_targets.find(id);
    if (it != m_targets.end() && it->second.size == requiredSize)
        return it->second;

    ViewTarget target;
    target.color = CreateColorTarget(requiredSize);
    target.depth = CreateDepthTarget(requiredSize);
    target.colorID = RenderGraph::CreateRGResourceID();
    target.depthID = RenderGraph::CreateRGResourceID();
    target.heapIndex = m_descFactory.CreateTextureSRV(target.color, RenderFormat::BackBufferFormat);
    target.size = requiredSize;

    auto [inserted, _] = m_targets.insert_or_assign(id, std::move(target));
    return inserted->second;
}

void ViewTargetResource::PruneUnused(const std::unordered_set<uint32_t>& activeViews)
{
    std::erase_if(m_targets, [&](auto& kv) { return !activeViews.contains(kv.first); });
}

Resource ViewTargetResource::CreateColorTarget(const Size& size)
{
    auto desc = CreateTextureDescriptor(size.width, size.height, RenderFormat::BackBufferFormat);
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = RenderFormat::BackBufferFormat;
    clearValue.Color[0] = clearValue.Color[1] = clearValue.Color[2] = clearValue.Color[3] = 0.0f; // PMA 컨벤션: 완전 투명 = RGBA 모두 0

    return m_device.CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue);
}

Resource ViewTargetResource::CreateDepthTarget(const Size& size)
{
    auto desc = CreateTextureDescriptor(size.width, size.height, DXGI_FORMAT_R32_TYPELESS);
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    return m_device.CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue);
}