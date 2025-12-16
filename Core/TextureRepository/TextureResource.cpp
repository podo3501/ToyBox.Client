#include "pch.h"
#include "TextureResource.h"

TextureResource::~TextureResource() = default;
TextureResource::TextureResource(ID3D12Device* device, DescriptorHeap* descHeap) noexcept :
	m_device{ device },
	m_srvDescriptors{ descHeap }
{}