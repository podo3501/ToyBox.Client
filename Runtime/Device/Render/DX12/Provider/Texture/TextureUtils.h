#pragma once
#include <d3d12.h>

struct TextureAsset;
class Resource;
class CommandList;

D3D12_RESOURCE_DESC CreateTexture2DDesc(const TextureAsset& asset, bool mips);
bool CanGenerateMips(const TextureAsset& asset, bool generateMips);
void UploadTexture(
    CommandList& uploadCmd,
    const TextureAsset& asset,
    Resource& texRes,
    Resource& uploadRes,
    UINT64 offset);