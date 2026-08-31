#include "pch.h"
#include "DescriptorFactory.h"
#include "Core/Device.h"
#include "Core/D3D12Conversions.h"
#include "Resource/Texture/TextureResource.h"
#include "Resource/Texture/TextureCubeResource.h"
#include "GameClient/Service/Render/RenderConfig.h"

DescriptorFactory::~DescriptorFactory() = default;
DescriptorFactory::DescriptorFactory(Device& device) :
    m_device{ device }
{}

bool DescriptorFactory::Initialize(const DescriptorConfig& config)
{
    ReturnIfFalse(m_bindlessAllocator.Initialize(m_device, config.bindless));
    ReturnIfFalse(m_rtvAllocator.Initialize(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, config.rtvCount));
    ReturnIfFalse(m_dsvAllocator.Initialize(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, config.dsvCount));

    return true;
}

UINT DescriptorFactory::CreateBufferSRV(
    DescriptorAllocationType type,
    const Resource& resBuffer,
    UINT firstElement, 
    UINT elementCount, 
    UINT elementStride)
{
    UINT index = UINT_MAX;
    switch (type)
    {
    case DescriptorAllocationType::Persistent: index = m_bindlessAllocator.AllocatePersistent(); break;
    case DescriptorAllocationType::Transient: index = m_bindlessAllocator.AllocateTransient(); break;
    case DescriptorAllocationType::Dynamic: Assert(false); break; //지원안함.
    }
    if (index == UINT_MAX)
        return UINT_MAX;

    auto srvDesc = CreateStructuredBufferSRVDesc(firstElement, elementCount, elementStride);
    m_device->CreateShaderResourceView(resBuffer.Get(), &srvDesc, GetBindlessCpuHandle(index));
    return index;
}

UINT DescriptorFactory::CreateTextureSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevels)
{
    const auto& resDesc = res->GetDesc();
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = mipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    UINT index = m_bindlessAllocator.AllocatePersistent();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateShaderResourceView(res.Get(), &srvDesc, GetBindlessCpuHandle(index));
    return index;
}

UINT DescriptorFactory::CreateTextureRTV(const Resource& res, DXGI_FORMAT format, UINT mipSlice)
{
    if (!res) return UINT_MAX;

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = format;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = mipSlice;
    rtvDesc.Texture2D.PlaneSlice = 0;

    UINT index = m_rtvAllocator.Allocate();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateRenderTargetView(res.Get(), &rtvDesc, GetRTVHandle(index));
    return index;
}

UINT DescriptorFactory::CreateTextureDSV(const Resource& res, DXGI_FORMAT format, UINT mipSlice)
{
    if (!res) return UINT_MAX;

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = format;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = mipSlice; // 기본값 0, 필요시 특정 밉슬라이스 지정 가능
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    // 앞서 설계한 DSV 전용 할당자(m_dsvAllocator)에서 공간 확보
    UINT index = m_dsvAllocator.Allocate();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateDepthStencilView(res.Get(), &dsvDesc, GetDSVHandle(index));
    return index;
}

bool DescriptorFactory::CreateTextureViews(
    TextureResource* texRes,
    bool generateMips,
    std::vector<UINT>* outMipSrvIndices,
    std::vector<UINT>* outMipUavIndices)
{
    if (!texRes) return false;

    auto& res = texRes->Get();
    const auto& resDesc = res->GetDesc();
    const UINT mipCount = resDesc.MipLevels;

    DXGI_FORMAT srvFormat = resDesc.Format;
    if (texRes->GetDesc().colorSpace == ColorSpace::SRGB)
        srvFormat = ToSRGB(resDesc.Format);

    UINT mainMipLevels = generateMips ? mipCount : 1;
    UINT mainIndex = CreateTextureSRV(res, srvFormat, mainMipLevels);
    if (mainIndex == UINT_MAX) return false;

    texRes->SetHeapIndex(mainIndex);

    if (generateMips && mipCount > 1)
    {
        if (outMipSrvIndices) outMipSrvIndices->reserve(mipCount);
        if (outMipUavIndices) outMipUavIndices->reserve(mipCount);

        for (UINT i = 0; i < mipCount; ++i)
        {
            UINT mipSrvIndex = CreateMipSRV(res, srvFormat, i);
            if (mipSrvIndex == UINT_MAX) return false;
            if (outMipSrvIndices) outMipSrvIndices->push_back(mipSrvIndex);

            UINT mipUavIndex = CreateMipUAV(res, resDesc.Format, i);
            if (mipUavIndex == UINT_MAX) return false;
            if (outMipUavIndices) outMipUavIndices->push_back(mipUavIndex);
        }
    }

    return true;
}

bool DescriptorFactory::CreateTextureCubeViews(TextureCubeResource* texRes)
{
    if (!texRes) return false;

    auto& res = texRes->Get();
    const auto& resDesc = res->GetDesc();
    const UINT mipCount = resDesc.MipLevels;

    DXGI_FORMAT srvFormat = resDesc.Format; // 큐브맵은 항상 Linear -> sRGB 변환 불필요 (일반 텍스처와 달리 감마 인코딩 없음)
    UINT index = CreateTextureCubeSRV(res, srvFormat, mipCount);
    if (index == UINT_MAX) return false;

    texRes->SetHeapIndex(index);
    return true;
}

UINT DescriptorFactory::CreateTextureCubeSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevels)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MipLevels = mipLevels;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

    UINT index = m_bindlessAllocator.AllocatePersistent();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateShaderResourceView(res.Get(), &srvDesc, GetBindlessCpuHandle(index));
    return index;
}

D3D12_SHADER_RESOURCE_VIEW_DESC DescriptorFactory::CreateStructuredBufferSRVDesc(
    UINT firstElement,
    UINT numElements, 
    UINT stride) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc{};

    desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = DXGI_FORMAT_UNKNOWN; // StructuredBuffer로 인식시키기 위해 Format은 UNKNOWN으로 설정

    desc.Buffer.FirstElement = firstElement;
    desc.Buffer.NumElements = numElements;
    desc.Buffer.StructureByteStride = stride; // 구조체(Vertex 등)의 크기
    desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    return desc;
}

UINT DescriptorFactory::CreateMipSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevel)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = mipLevel;  // 인자로 넘어온 타겟 밉슬라이스 고정
    srvDesc.Texture2D.MipLevels = 1;  // 무조건 1개 밉 레벨 영역만 타겟팅
    //srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    UINT index = m_bindlessAllocator.AllocateDynamic();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateShaderResourceView(res.Get(), &srvDesc, GetBindlessCpuHandle(index));
    return index;
}

UINT DescriptorFactory::CreateMipUAV(const Resource& res, DXGI_FORMAT format, UINT mipLevel)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = mipLevel;

    UINT index = m_bindlessAllocator.AllocateDynamic();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateUnorderedAccessView(res.Get(), nullptr, &uavDesc, GetBindlessCpuHandle(index));
    return index;
}

void DescriptorFactory::FreeRTV(UINT rtvIndex)
{
    m_rtvAllocator.Free(rtvIndex);
}

void DescriptorFactory::FreeDSV(UINT dsvIndex)
{
    m_dsvAllocator.Free(dsvIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorFactory::GetRTVHandle(UINT rtvIndex)
{
    return m_rtvAllocator.GetCpuHandle(rtvIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorFactory::GetDSVHandle(UINT dsvIndex)
{
    return m_dsvAllocator.GetCpuHandle(dsvIndex);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorFactory::GetBindlessCpuHandle(UINT index)
{
    return m_bindlessAllocator.GetCpuHandle(index);
}
D3D12_GPU_DESCRIPTOR_HANDLE DescriptorFactory::GetBindlessGpuHandle(UINT index)
{
    return m_bindlessAllocator.GetGpuHandle(index);
}
