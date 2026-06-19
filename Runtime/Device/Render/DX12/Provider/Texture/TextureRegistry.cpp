#include "pch.h"
#include "TextureRegistry.h"

void TextureRegistry::Register(uint32_t id, std::shared_ptr<ITextureResource> resource)
{
    m_textures[id] = resource;
}

TextureResource* TextureRegistry::GetTextureResource(uint32_t id)
{
    auto it = m_textures.find(id);
    if (it == m_textures.end()) return nullptr;

    auto res = it->second.lock();
    if (!res)
        return nullptr;

    return static_cast<TextureResource*>(res.get());
}

void TextureRegistry::FinalizeTexture(uint32_t id)
{
    auto it = m_textures.find(id);
    if (it == m_textures.end()) return;
    
    auto res = it->second.lock();
    if (!res)
        return;

    auto* texRes = static_cast<TextureResource*>(res.get());
    texRes->MarkReady();
}