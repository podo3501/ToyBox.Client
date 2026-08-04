#pragma once
#include <d3d12.h>

struct TextureAsset;
struct TextureCubeAsset;
class Resource;
class CommandList;

D3D12_RESOURCE_DESC CreateTexture2DDesc(const TextureAsset& asset, bool mips);
D3D12_RESOURCE_DESC CreateTextureCubeDesc(const TextureCubeAsset& asset);
bool CanGenerateMips(const TextureAsset& asset, bool generateMips);

void UploadTexture(
    CommandList& uploadCmd,
    const TextureAsset& asset,
    Resource& texRes,
    Resource& uploadRes,
    UINT64 offset);

void UploadTextureCube(
    CommandList& uploadCmd,
    const TextureCubeAsset& asset,
    Resource& texRes,
    Resource& uploadRes,
    UINT64 offset);