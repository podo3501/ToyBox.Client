#include "pch.h"
#include "TextureSystem.h"
#include "TextureGraphBuilder.h"
#include "TextureResource.h"

TextureSystem::~TextureSystem() = default;
TextureSystem::TextureSystem(TextureGraphBuilder* builder, TextureRegistry* registry) :
    m_builder{ builder },
    m_registry{ registry }
{}

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
