#include "pch.h"
#include "DescriptorFactory.h"
#include "DescriptorAllocator.h"
#include "GameClient/Service/Render/TextureDesc.h"

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

DescriptorAllocation DescriptorFactory::CreateSRV(ID3D12Resource* res, const TextureDesc& desc, bool generateMips)
{
    const auto& resDesc = res->GetDesc();
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = desc.srgb ? MakeSRGBFormat(resDesc.Format) : resDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = generateMips ? resDesc.MipLevels : 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    auto allocation = m_srvAllocator->Allocate();
    if (!allocation.IsValid()) return {};

    m_device->CreateShaderResourceView(res, &srvDesc, allocation.GetCpuHandle());
    return allocation;
}