#include "pch.h"
#include "ResourceUploader.h"
#include "CommandScheduler.h"
#include "DX12DeviceView.h"
#include "ImageData.h"
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

ResourceUploader::~ResourceUploader() = default;
ResourceUploader::ResourceUploader(const DX12DeviceView& dv) :
    m_dv{ dv }
{}

ComPtr<ID3D12Resource> ResourceUploader::UploadTexture(
    const ImageData& img,
    ComPtr<ID3D12Resource>& outUploadBuffer,
    ID3D12GraphicsCommandList* uploadCmd)
{
    auto device = m_dv.device;

    // 1. 텍스처 생성 (DEFAULT heap)
    D3D12_RESOURCE_DESC texDesc{};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = img.width;
    texDesc.Height = img.height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    ComPtr<ID3D12Resource> texture;

    CD3DX12_HEAP_PROPERTIES defaultHeap(D3D12_HEAP_TYPE_DEFAULT);
    device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&texture));

    // 2. 업로드 버퍼 생성
    UINT64 uploadSize = 0;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0,
        nullptr, nullptr, nullptr, &uploadSize);

    CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

    device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&outUploadBuffer));

    //auto toCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
    //    texture.Get(),
    //    D3D12_RESOURCE_STATE_COMMON,
    //    D3D12_RESOURCE_STATE_COPY_DEST);
    //uploadCmd->ResourceBarrier(1, &toCopyDest);

    // 3. 데이터 복사
    D3D12_SUBRESOURCE_DATA subresource{};
    subresource.pData = img.pixels.data();
    subresource.RowPitch = img.stride;
    subresource.SlicePitch = img.stride * img.height;

    UpdateSubresources(uploadCmd,
        texture.Get(),
        outUploadBuffer.Get(),
        0, 0, 1,
        &subresource);

    //// 4. 상태 전환
    //auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
    //    texture.Get(),
    //    D3D12_RESOURCE_STATE_COPY_DEST,
    //    D3D12_RESOURCE_STATE_COMMON);

    //uploadCmd->ResourceBarrier(1, &barrier);

    return texture;
}