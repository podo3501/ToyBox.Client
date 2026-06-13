#pragma once
#include "Resource.h"

struct UploadLayout 
{
    UINT64 totalBytes;
    UINT64 alignedTotalBytes;
};

struct UploadRegion
{
    const void* data{ nullptr }; //보낼 정보
    UINT64 size{ 0 };
    UINT64 srcOffset{ 0 }; //upload 안에 offset
    Resource dstBuffer;
};

struct TextureAsset;
class Device;
class CommandList;

class ResourceLoader
{
public:
    ~ResourceLoader();
    ResourceLoader(Device& device);

    //?!? create 되는 함수는 Factory 클래스로 이동하는게 좋을듯.
    Resource CreateUploadResource(size_t size);
    Resource CreateTextureResource(const D3D12_RESOURCE_DESC& desc);
    Resource CreateBufferResource(UINT64 size);
    Resource CreateShadowResource(UINT width, UINT height);

    bool ShouldGenerateMips(const TextureAsset& asset, bool generateMips);
    D3D12_RESOURCE_DESC CreateTexture2DDesc(const TextureAsset& asset, bool mips);
    UINT64 GetTextureUploadLayout(const D3D12_RESOURCE_DESC& texDesc, size_t offset);

    void UploadBufferRegion(
        CommandList& cmd, 
        Resource& uploadRes,
        const UploadRegion& region);

    void UploadTexture(
        CommandList& uploadCmd,
        const TextureAsset& asset,
        Resource& texRes,
        Resource& uploadRes,
        UINT64 offset);

private:
    Resource CreateResource(
        const D3D12_RESOURCE_DESC& desc,
        D3D12_HEAP_TYPE heapType,
        D3D12_RESOURCE_STATES state,
        const D3D12_CLEAR_VALUE* clearValue = nullptr);

    Device& m_device;
};