#pragma once
#include <wrl/client.h>
#include "d3dx12.h"

struct MeshBundle //?!? 리팩토링하면서 다른 이름으로 바꾸자.
{
    Microsoft::WRL::ComPtr<ID3D12Resource> vb{ nullptr };
    Microsoft::WRL::ComPtr<ID3D12Resource> ib{ nullptr };
};

struct UploadBuffer
{
    Microsoft::WRL::ComPtr<ID3D12Resource> resource{ nullptr };
};

struct UploadableResource
{
    Microsoft::WRL::ComPtr<ID3D12Resource> res;
    Microsoft::WRL::ComPtr<ID3D12Resource> upload;
};

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
    ID3D12Resource* dstBuffer{ nullptr };
};

struct TextureAsset;
struct MeshAsset;
struct Vertex;
class CommandList;

using Microsoft::WRL::ComPtr;

class ResourceLoader
{
public:
    ~ResourceLoader();
    ResourceLoader(ID3D12Device* device);

    std::pair<UploadableResource, bool> CreateUploadableTexture(
        const TextureAsset& asset, 
        bool generateMips);

    ComPtr<ID3D12Resource> CreateUploadResource(size_t size);
    bool ShouldGenerateMips(const TextureAsset& asset, bool generateMips);
    D3D12_RESOURCE_DESC CreateTexture2DDesc(const TextureAsset& asset, bool mips);
    ComPtr<ID3D12Resource> CreateTextureResource(const D3D12_RESOURCE_DESC& desc);
    UINT64 GetTextureUploadLayout(const D3D12_RESOURCE_DESC& texDesc, size_t offset);
    ComPtr<ID3D12Resource> CreateBufferResource(UINT64 size);
    void UploadBufferRegion(CommandList& cmd, ID3D12Resource* uploadRes, const UploadRegion& region);




    void UploadTexture(
        CommandList& uploadCmd,
        const TextureAsset& asset,
        ID3D12Resource* texRes,
        ID3D12Resource* uploadRes,
        UINT64 offset);

    void UploadTexture(
        CommandList& uploadCmd, 
        const TextureAsset& asset, 
        UploadableResource uploadableRes);

    ComPtr<ID3D12Resource> UploadVertexBuffer(
        CommandList& cmd,
        const void* data,
        UINT bufferSize,
        ComPtr<ID3D12Resource>& outUploadBuffer);

    MeshBundle UploadMesh(
        CommandList& uploadCmd,
        const MeshAsset& asset,
        UploadBuffer& outUploadBuffer);

    ComPtr<ID3D12Resource> UploadVertexBuffer(
        CommandList& cmd,
        const std::vector<Vertex>& vertices,
        UploadBuffer& outUploadBuffer);

    ComPtr<ID3D12Resource> UploadIndexBuffer(
        CommandList& cmd,
        const std::vector<uint32_t>& indices,
        UploadBuffer& outUploadBuffer);

private:
    ComPtr<ID3D12Resource> CreateResource(
        const D3D12_RESOURCE_DESC& desc,
        D3D12_HEAP_TYPE heapType,
        D3D12_RESOURCE_STATES state);

    ID3D12Device* m_device{ nullptr };
};