#include "pch.h"
#include "TextureRepository.h"
#include "CommandScheduler.h"
#include "DescriptorAllocator.h"
#include "ResourceUploader.h"

TextureRepository::~TextureRepository() = default;
TextureRepository::TextureRepository(ID3D12Device* device, CommandScheduler* command,
    DescriptorAllocator* srvAllocator, ResourceUploader* uploader) :
    m_device{ device },
    m_command{ command },
    m_srvAllocator{ srvAllocator },
    m_uploader{ uploader }
{}

int TextureRepository::Upload(const TextureAsset& asset)
{
    auto cmd = m_command->Begin(CommandType::Copy);

    ComPtr<ID3D12Resource> uploadBuffer;
    auto texture = m_uploader->UploadTexture(cmd, asset, uploadBuffer);

    m_command->End({ uploadBuffer });

    return AddTexture(texture);
}

const GpuTexture* TextureRepository::GetTexture(int index) const
{ 
    if (index < 0 || index >= static_cast<int>(m_textureResources.size()))
        return nullptr;

    return &m_textureResources[index];
}

int TextureRepository::AddTexture(ComPtr<ID3D12Resource> tex)
{
    UINT index = m_srvAllocator->Allocate();
    auto cpuHandle = m_srvAllocator->GetCpuHandle(index);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = tex->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    m_device->CreateShaderResourceView(tex.Get(), &srvDesc, cpuHandle);

    m_textureResources.push_back({ tex, index });
    return static_cast<int>(m_textureResources.size() - 1);
}