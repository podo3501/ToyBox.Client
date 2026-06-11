#include "pch.h"
#include "ShadowResource.h"
#include "ResourceLoader.h"
#include "../Descriptor/DescriptorFactory.h"


bool ShadowResource::Initialize(ResourceLoader* loader, DescriptorFactory* factory, UINT width, UINT height)
{
	m_resource = Resource{ loader->CreateShadowResource(width, height) };
	if (!m_resource) return false;

	m_dsvIndex = factory->CreateTextureDSV(m_resource, DXGI_FORMAT_D32_FLOAT);
	m_srvIndex = factory->CreateTextureSRV(m_resource, DXGI_FORMAT_R32_FLOAT);

	if (m_dsvIndex == UINT_MAX || m_srvIndex == UINT_MAX)
		return false;

	return true;
}