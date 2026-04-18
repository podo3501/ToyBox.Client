#pragma once
#include <wrl/client.h>
#include "d3dx12.h"

struct TextureAsset;
class CommandList;

using Microsoft::WRL::ComPtr;

class ResourceUploader
{
public:
    ~ResourceUploader();
    ResourceUploader(ID3D12Device* device);

    std::pair<ComPtr<ID3D12Resource>, bool> UploadTexture(
        CommandList* uploadCmd,
        const TextureAsset& asset,
        bool generateMips,
        ComPtr<ID3D12Resource>& outUploadBuffer);

    ComPtr<ID3D12Resource> UploadVertexBuffer(
        CommandList* cmd,
        const void* data,
        UINT bufferSize,
        ComPtr<ID3D12Resource>& outUploadBuffer);

private:
    ComPtr<ID3D12Resource> CreateResource(
        const D3D12_RESOURCE_DESC& desc,
        D3D12_HEAP_TYPE heapType,
        D3D12_RESOURCE_STATES state);

    ID3D12Device* m_device{ nullptr };
};