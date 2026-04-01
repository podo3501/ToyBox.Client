#pragma once
#include <wrl/client.h>
#include "DX12DeviceView.h"

struct ID3D12Resource;
struct ID3D12GraphicsCommandList;
class CommandScheduler;
struct ImageData;

class ResourceUploader
{
public:
    ~ResourceUploader();
    ResourceUploader() = delete;
    ResourceUploader(const DX12DeviceView& dv);
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTexture(
        const ImageData& img,
        Microsoft::WRL::ComPtr<ID3D12Resource>& outUploadBuffer,
        ID3D12GraphicsCommandList* uploadCmd);

private:
    DX12DeviceView m_dv{};
};