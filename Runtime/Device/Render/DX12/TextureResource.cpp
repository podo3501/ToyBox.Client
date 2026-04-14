#include "pch.h"
#include "TextureResource.h"
#include "CommandScheduler.h"
#include "DescriptorAllocator.h"
#include "ResourceUploader.h"
#include "GameClient/Service/Asset/Assets/TextureAsset.h"

TextureResource::~TextureResource()
{
    if (m_srvAllocator && m_srvIndex != UINT_MAX)
        m_srvAllocator->DeferredFree(m_srvIndex, m_command->GetLastSubmittedFences());
}

TextureResource::TextureResource(ID3D12Device* device, CommandScheduler* command,
    DescriptorAllocator* srvAllocator, ResourceUploader* uploader) :
    m_device{ device },
    m_command{ command },
    m_srvAllocator{ srvAllocator },
    m_uploader{ uploader }
{}

bool TextureResource::LoadFromAsset(shared_ptr<TextureAsset> asset)
{
    UINT index = m_srvAllocator->Allocate();
    if (index == UINT_MAX)
        return false;

    auto cmd = m_command->Begin(CommandType::Copy);
    if (!cmd) return false; 

    ComPtr<ID3D12Resource> uploadBuffer;
    auto texture = m_uploader->UploadTexture(cmd, *asset, uploadBuffer);

    m_command->End({ uploadBuffer });

    AddTexture(index, texture);
    return true;
}

void TextureResource::AddTexture(UINT index, ComPtr<ID3D12Resource> tex)
{
    auto cpuHandle = m_srvAllocator->GetCpuHandle(index);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = tex->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    m_device->CreateShaderResourceView(tex.Get(), &srvDesc, cpuHandle);

    m_tex = tex;
    m_srvIndex = index;
}
