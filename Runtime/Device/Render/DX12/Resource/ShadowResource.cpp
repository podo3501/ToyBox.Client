#include "pch.h"
#include "ShadowResource.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Device.h"
#include "Factory/ResourceFactory.h"
#include "Factory/DescriptorFactory.h"
#include "Helpers/TextureHelpers.h"

static Resource CreateShadowResource(Device& device, const Size& shadowMapSize)
{
    auto desc = CreateTextureDescriptor(shadowMapSize.width, shadowMapSize.height, DXGI_FORMAT_R32_TYPELESS);
    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    return device.CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue);
}

bool ShadowResource::Initialize(Device& device, DescriptorFactory& factory, const Size& shadowMapSize)
{
	m_resource = CreateShadowResource(device, shadowMapSize);
	if (!m_resource) return false;

	m_dsvIndex = factory.CreateTextureDSV(m_resource, DXGI_FORMAT_D32_FLOAT);
	m_srvIndex = factory.CreateTextureSRV(m_resource, DXGI_FORMAT_R32_FLOAT);

	if (m_dsvIndex == UINT_MAX || m_srvIndex == UINT_MAX)
		return false;

	return true;
}