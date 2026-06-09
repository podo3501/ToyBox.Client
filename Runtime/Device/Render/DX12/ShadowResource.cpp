#include "pch.h"
#include "ShadowResource.h"
#include "Descriptor/DescriptorFactory.h"
#include "ResourceLoader.h"

bool ShadowResource::Initialize(ResourceLoader* loader, DescriptorFactory* factory, UINT width, UINT height)
{
	m_resource = loader->CreateShadowResource(width, height);
	if (!m_resource) return false;

	m_dsvIndex = factory->CreateTextureDSV(m_resource.Get(), DXGI_FORMAT_D32_FLOAT);
	m_srvIndex = factory->CreateTextureSRV(m_resource.Get(), DXGI_FORMAT_R32_FLOAT);

	if (m_dsvIndex == UINT_MAX || m_srvIndex == UINT_MAX)
		return false;

	return true;
}