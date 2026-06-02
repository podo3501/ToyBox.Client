#include "pch.h"
#include "TextureResource.h"

TextureResource::~TextureResource() = default;
TextureResource::TextureResource(const TextureDesc& desc)
{
	m_desc = desc;
}

