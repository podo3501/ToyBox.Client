#include "pch.h"
#include "DescriptorFactory.h"
#include "Core/Device.h"
#include "Core/D3D12Conversions.h"
#include "Resource/Texture/TextureResource.h"
#include "Resource/Resource.h"
#include "GameClient/Service/Render/Desc/TextureDesc.h"
#include "GameClient/Service/Render/RenderConfig.h"

DescriptorFactory::~DescriptorFactory() = default;
DescriptorFactory::DescriptorFactory(Device& device) :
    m_device{ device }
{}

bool DescriptorFactory::Initialize(const DescriptorConfig& config)
{
    ReturnIfFalse(m_bindlessAllocator.Initialize(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, config.bindlessCount));
    ReturnIfFalse(m_dsvAllocator.Initialize(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, config.dsvCount));

    return true;
}

UINT DescriptorFactory::CreateBufferSRV(const Resource& resBuffer, UINT elementCount, UINT elementStride)
{
    auto srvDesc = CreateStructuredBufferSRVDesc(elementCount, elementStride);

    UINT index = m_bindlessAllocator.Allocate();
    if (index == UINT_MAX)
        return UINT_MAX;

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

    UINT Index = m_bindlessAllocator.Allocate();
    if (Index == UINT_MAX) return UINT_MAX;

    m_device->CreateShaderResourceView(res.Get(), &srvDesc, GetBindlessCpuHandle(Index));
    return Index;
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

bool DescriptorFactory::CreateTextureViews(TextureResource* texRes, bool generateMips)
{
    if (!texRes) return false;

    auto& res = texRes->Get();
    const auto& resDesc = res->GetDesc();
    const UINT mipCount = resDesc.MipLevels;

    DXGI_FORMAT srvFormat = resDesc.Format;
    if (texRes->GetDesc().type == TextureType::Color)
        srvFormat = ToSRGB(resDesc.Format);

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

        texRes->SetMipSRVIndices(std::move(mipSrvIndices));
        texRes->SetMipUAVIndices(std::move(mipUavIndices));
    }

    return true;
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

UINT DescriptorFactory::CreateMipSRV(const Resource& res, DXGI_FORMAT format, UINT mipLevel)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = mipLevel;  // 인자로 넘어온 타겟 밉슬라이스 고정
    srvDesc.Texture2D.MipLevels = 1;  // 무조건 1개 밉 레벨 영역만 타겟팅
    //srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    UINT index = m_bindlessAllocator.AllocateTransient();
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

    UINT index = m_bindlessAllocator.AllocateTransient();
    if (index == UINT_MAX) return UINT_MAX;

    m_device->CreateUnorderedAccessView(res.Get(), nullptr, &uavDesc, GetBindlessCpuHandle(index));
    return index;
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
