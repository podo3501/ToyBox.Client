#pragma once
#include "d3d12.h"

constexpr size_t AlignVertexIndex = 16;
constexpr size_t AlignConstantBuffer = 256;
constexpr size_t AlignTexture = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT; // 512

size_t AlignSize(size_t value, size_t alignment);
