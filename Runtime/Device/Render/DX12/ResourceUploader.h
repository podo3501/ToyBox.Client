#pragma once
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

struct TextureAsset;

class ResourceUploader
{
public:
    ~ResourceUploader();
    ResourceUploader(ID3D12Device* device);

    ComPtr<ID3D12Resource> UploadTexture(
        ID3D12GraphicsCommandList* uploadCmd,
        const TextureAsset& asset,
        ComPtr<ID3D12Resource>& outUploadBuffer);

    ComPtr<ID3D12Resource> UploadVertexBuffer(
        ID3D12GraphicsCommandList* cmd,
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