#include "pch.h"
#include "TextureResource.h"
#include "CommandScheduler.h"
#include "DescriptorAllocator.h"
#include "ResourceUploader.h"
#include "ResourcePreparer.h"
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "GameClient/Service/Render/TextureDesc.h"

TextureResource::~TextureResource()
{
    if (m_srvAllocator && m_srvIndex != UINT_MAX)
        m_srvAllocator->DeferredFree(m_srvIndex, m_command->GetLastSubmittedFences());
}

TextureResource::TextureResource(ID3D12Device* device, CommandScheduler* command,
    DescriptorAllocator* srvAllocator, ResourceUploader* uploader, ResourcePreparer* preparer) :
    m_device{ device },
    m_command{ command },
    m_srvAllocator{ srvAllocator },
    m_uploader{ uploader },
    m_preparer{ preparer }
{}

bool TextureResource::LoadFromAsset(shared_ptr<TextureAsset> asset, const TextureDesc& desc)
{
    UINT index = m_srvAllocator->Allocate();
    if (index == UINT_MAX)
        return false;

    auto cmd = m_command->Begin(CommandType::Copy);
    if (!cmd) return false; 

    ComPtr<ID3D12Resource> uploadBuffer;
    auto texture = m_uploader->UploadTexture(cmd, *asset, desc.generateMips, uploadBuffer);

    auto submitFence = m_command->End({ uploadBuffer });

    m_preparer->Enqueue({ texture.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        submitFence, this });

    AddTexture(index, texture, desc);
    return true;
}

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

void TextureResource::AddTexture(UINT index, ComPtr<ID3D12Resource> tex, const TextureDesc& desc)
{
    auto cpuHandle = m_srvAllocator->GetCpuHandle(index);
    const auto texDesc = tex->GetDesc();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = desc.srgb ? MakeSRGBFormat(texDesc.Format) : texDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    if (desc.generateMips)
    {
        srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    }
    else
    {
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    }

    m_device->CreateShaderResourceView(tex.Get(), &srvDesc, cpuHandle);

    m_srvIndex = index;
}