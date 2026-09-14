#pragma once
#include "d3d12.h"

namespace BufferAlignment
{
	constexpr size_t VertexBuffer = 16;
	constexpr size_t IndexBuffer = 16;
	constexpr size_t ConstantBuffer = 256;
}

namespace TextureAlignment
{
	constexpr size_t Row = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT; // 256
	constexpr size_t Placement = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT; // 512
}

constexpr uint32_t FrameBufferCount = 3;