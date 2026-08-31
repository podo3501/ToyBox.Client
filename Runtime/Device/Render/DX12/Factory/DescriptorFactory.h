#pragma once
#include "d3dx12.h"
#include "Allocator/DescriptorAllocationType.h"
#include "Allocator/BindlessDescriptorAllocator.h"
#include "Allocator/DescriptorAllocator.h"

struct DescriptorConfig;
class Device;
class Resource;
class TextureResource;
class TextureCubeResource;
class BindlessDescriptorAllocator;

class DescriptorFactory
{
public:
    ~DescriptorFactory();
    DescriptorFactory() = delete;
    explicit DescriptorFactory(Device& device);
    bool Initialize(const DescriptorConfig& config);
    UINT CreateBufferSRV(
        DescriptorAllocationType type,
        const Resource& resBuffer, 
        UINT firstElement, 
        UINT elementCount, 
        UINT elementStride); //vb, ib 만들때
    UINT CreateTextureSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevels = 1);
    UINT CreateTextureRTV(const Resource& res, DXGI_FORMAT format, UINT mipSlice = 0);
    UINT CreateTextureDSV(const Resource& res, DXGI_FORMAT format, UINT mipSlice = 0);
    bool CreateTextureViews(
        TextureResource* texRes,
        bool generateMips,
        std::vector<UINT>* outMipSrvIndices = nullptr,
        std::vector<UINT>* outMipUavIndices = nullptr);
    bool CreateTextureCubeViews(TextureCubeResource* texRes);

    void FreeRTV(UINT rtvIndex);
    void FreeDSV(UINT dsvIndex);

    BindlessDescriptorAllocator& GetBindlessAllocator() noexcept { return m_bindlessAllocator; }
    DescriptorAllocator& GetDSVAllocator() noexcept { return m_dsvAllocator; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(UINT rtvIndex);
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(UINT index); //dsv는 gpu 핸들 api를 제공하지 않는다.
    D3D12_CPU_DESCRIPTOR_HANDLE GetBindlessCpuHandle(UINT index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetBindlessGpuHandle(UINT index);

private:
    D3D12_SHADER_RESOURCE_VIEW_DESC CreateStructuredBufferSRVDesc(
        UINT firstElement, 
        UINT numElements, 
        UINT stride) const;
    UINT CreateMipSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevel);
    UINT CreateMipUAV(const Resource& res, DXGI_FORMAT format, UINT mipLevel);
    UINT CreateTextureCubeSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevels);

    Device& m_device;
    BindlessDescriptorAllocator m_bindlessAllocator; // srv/uav/cbv 셋다 하나의 큰 힙에 들어감. cbv는 거의 안씀.
    DescriptorAllocator m_rtvAllocator;
    DescriptorAllocator m_dsvAllocator;
};
