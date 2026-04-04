#include "pch.h"
#include "TextureRepository.h"
#include "TextureLoader.h"
#include "CommandScheduler.h"
#include "DescriptorAllocator.h"
#include "ResourceUploader.h"
#include "ImageData.h"

TextureRepository::~TextureRepository() = default;
TextureRepository::TextureRepository(ID3D12Device* device, CommandScheduler* command,
    DescriptorAllocator* srvAllocator, ResourceUploader* uploader) :
    m_device{ device },
    m_command{ command },
    m_srvAllocator{ srvAllocator },
    m_uploader{ uploader }
{
    m_loader = make_unique<TextureLoader>();
}

int TextureRepository::LoadFromMemory(Core::ByteBuffer buffer)
{
    ImageData img = Decode(move(buffer));
    ComPtr<ID3D12Resource> texture = Upload(img);

    return AddTexture(texture);
}

const GpuTexture* TextureRepository::GetTexture(int index) const
{ 
    if (index < 0 || index >= static_cast<int>(m_textureResources.size()))
        return nullptr;

    return &m_textureResources[index];
}

ImageData TextureRepository::Decode(Core::ByteBuffer buffer)
{
    return m_loader->LoadFromMemory(std::move(buffer));
}

ComPtr<ID3D12Resource> TextureRepository::Upload(ImageData& img)
{
    auto cmd = m_command->Begin(CommandType::Copy);

    ComPtr<ID3D12Resource> uploadBuffer;
    auto texture = m_uploader->UploadTexture(cmd, img, uploadBuffer);

    m_command->End({ uploadBuffer });

    return texture;
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