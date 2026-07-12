#pragma once
#include "d3d12.h"

constexpr size_t AlignVertexBuffer = 16;
constexpr size_t AlignIndexBuffer = 16;
constexpr size_t AlignConstantBuffer = 256;
constexpr size_t AlignTextureRow = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT; // 256
constexpr size_t AlignTexturePlacement = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT; // 512