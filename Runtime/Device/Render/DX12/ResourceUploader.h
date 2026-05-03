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

struct TextureAsset;
struct MeshAsset;
struct Vertex;
class CommandList;

using Microsoft::WRL::ComPtr;

class ResourceUploader
{
public:
    ~ResourceUploader();
    ResourceUploader(ID3D12Device* device);
    bool ShouldGenerateMips(const TextureAsset& asset, bool generateMips);

    ComPtr<ID3D12Resource> UploadTexture(
        CommandList& uploadCmd,
        const TextureAsset& asset,
        bool generateMips,
        UploadBuffer& outUploadBuffer);

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