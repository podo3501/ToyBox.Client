#include "pch.h"
#include "TextureSystem.h"
#include "MipGenerator.h"
#include "DescriptorFactory.h"
#include "TextureGraphBuilder.h"
#include "TextureResource.h"

TextureSystem::~TextureSystem() = default;
TextureSystem::TextureSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TaskScheduler* taskScheduler, ResourceUploader* uploader) :
    m_mipGenerator{ make_unique<MipGenerator>(device, srvAllocator) },
    m_descriptorFactory{ make_unique<DescriptorFactory>(device, srvAllocator) },
    m_registry{ make_unique<TextureRegistry>() },
    m_builder{ make_unique<TextureGraphBuilder>(taskScheduler, uploader, 
        m_mipGenerator.get(), m_descriptorFactory.get(), m_registry.get()) }
{}

bool TextureSystem::Initialize()
{
    return m_mipGenerator->Initialize();
}

shared_ptr<ITextureResource> TextureSystem::CreateTextureResource()
{
    return make_shared<TextureResource>();
}

bool TextureSystem::LoadFromAsset(std::shared_ptr<ITextureResource> resource, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc)
{
    RGTexture tex = m_builder->LoadTexture(asset, desc);
    m_registry->Register(tex.id, resource);

    return true;
}
