#include "pch.h"
#include "MaterialResource.h"

MaterialResource::~MaterialResource() = default;
MaterialResource::MaterialResource(std::shared_ptr<ITextureResource> texRes)
{
	m_texRes = texRes;
}