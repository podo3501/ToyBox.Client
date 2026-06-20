#pragma once
#include <d3d12.h>

D3D12_RESOURCE_DESC CreateTextureDescriptor(UINT64 width, UINT height, DXGI_FORMAT format);