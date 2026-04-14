#include "pch.h"
#include "TextureRes.h"

TextureRes::~TextureRes() = default;
TextureRes::TextureRes(ID3D12Device* device, DescriptorHeap* descHeap) noexcept :
	m_device{ device },
	m_srvDescriptors{ descHeap }
{}