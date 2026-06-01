#include "pch.h"
#include "DescriptorFactory.h"
#include "DescriptorAllocator.h"
#include "../TextureResource.h"
#include "GameClient/Service/Render/Desc/TextureDesc.h"

DescriptorFactory::~DescriptorFactory() = default;
DescriptorFactory::DescriptorFactory(ID3D12Device* device, DescriptorAllocator* srvAllocator) :
    m_device{ device },
    m_srvAllocator{ srvAllocator }
{}

static DXGI_FORMAT MakeSRGBFormat(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case DXGI_FORMAT_BC1_UNORM: return DXGI_FORMAT_BC1_UNORM_SRGB;
    case DXGI_FORMAT_BC2_UNORM: return DXGI_FORMAT_BC2_UNORM_SRGB;
    case DXGI_FORMAT_BC3_UNORM: return DXGI_FORMAT_BC3_UNORM_SRGB;
    default:
        return format; // 이미 SRGB거나 변환 불가
    }
}

bool DescriptorFactory::CreateTextureViews(TextureResource* texRes, const TextureDesc& desc, bool generateMips)
{
    if (!texRes) return false;

    ID3D12Resource* res = texRes->Get();
    const auto& resDesc = res->GetDesc();
    const UINT mipCount = resDesc.MipLevels;

    DXGI_FORMAT srvFormat = desc.srgb ? MakeSRGBFormat(resDesc.Format) : resDesc.Format;

    UINT mainMipLevels = generateMips ? mipCount : 1;
    UINT mainIndex = CreateTextureSRV(res, srvFormat, mainMipLevels);
    if (mainIndex == UINT_MAX) return false;

    texRes->SetHeapIndex(mainIndex);

    if (generateMips && mipCount > 1)
    {
        std::vector<UINT> mipSrvIndices;
        std::vector<UINT> mipUavIndices;
        mipSrvIndices.reserve(mipCount);
        mipUavIndices.reserve(mipCount);

        for (UINT i = 0; i < mipCount; ++i)
        {
            UINT mipSrvIndex = CreateMipSRV(res, srvFormat, i);
            if (mipSrvIndex == UINT_MAX) return false;
            mipSrvIndices.push_back(mipSrvIndex);

            UINT mipUavIndex = CreateMipUAV(res, resDesc.Format, i); //UAV는 sRGB 포맷을 쓸 수 없으므로 원본 포맷 리스 사용
            if (mipUavIndex == UINT_MAX) return false;
            mipUavIndices.push_back(mipUavIndex);
        }

        texRes->SetMipSrvIndices(std::move(mipSrvIndices));
        texRes->SetMipUavIndices(std::move(mipUavIndices));
    }

    return true;
}

UINT DescriptorFactory::CreateTextureSRV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevels)
{
    const auto& resDesc = res->GetDesc();
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = mipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    UINT Index = m_srvAllocator->Allocate();
    if (Index == UINT_MAX) return UINT_MAX;

    m_device->CreateShaderResourceView(res, &srvDesc, m_srvAllocator->GetCpuHandle(Index));
    return Index;
}

UINT DescriptorFactory::CreateMipSRV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevel)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;  // 무조건 1개 밉 레벨 영역만 타겟팅
    srvDesc.Texture2D.MostDetailedMip = mipLevel;  // 인자로 넘어온 타겟 밉슬라이스 고정
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    UINT index = m_srvAllocator->AllocateTransient();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateShaderResourceView(res, &srvDesc, m_srvAllocator->GetCpuHandle(index));
    return index;
}

UINT DescriptorFactory::CreateMipUAV(ID3D12Resource* res, DXGI_FORMAT format, UINT mipLevel)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = mipLevel;

    UINT index = m_srvAllocator->AllocateTransient();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateUnorderedAccessView(res, nullptr, &uavDesc, m_srvAllocator->GetCpuHandle(index));
    return index;
}

UINT DescriptorFactory::CreateBufferSRV(ID3D12Resource* buffer, UINT elementCount, UINT elementStride)
{
    auto srvDesc = CreateStructuredBufferSRVDesc(elementCount, elementStride);

    UINT index = m_srvAllocator->Allocate();
    if (index == UINT_MAX)
        return UINT_MAX;

    m_device->CreateShaderResourceView(buffer, &srvDesc, m_srvAllocator->GetCpuHandle(index));
    return index;
}

D3D12_SHADER_RESOURCE_VIEW_DESC DescriptorFactory::CreateStructuredBufferSRVDesc(
    UINT numElements, UINT stride) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc{};

    desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = DXGI_FORMAT_UNKNOWN; // StructuredBuffer로 인식시키기 위해 Format은 UNKNOWN으로 설정

    desc.Buffer.FirstElement = 0;
    desc.Buffer.NumElements = numElements;
    desc.Buffer.StructureByteStride = stride; // 구조체(Vertex 등)의 크기
    desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    return desc;
}