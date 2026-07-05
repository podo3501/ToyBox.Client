#include "pch.h"
#include "TextureRegistry.h"

void TextureRegistry::Register(RGResourceID resID, std::shared_ptr<TextureResource> resource)
{
    m_textures[resID] = resource;
}

TextureResource* TextureRegistry::GetTextureResource(RGResourceID resID)
{
    auto it = m_textures.find(resID);
    if (it == m_textures.end()) return nullptr;

    auto res = it->second.lock();
    if (!res)
        return nullptr;

    return static_cast<TextureResource*>(res.get());
}

void TextureRegistry::FinalizeTexture(RGResourceID resID)
{
    auto it = m_textures.find(resID);
    if (it == m_textures.end()) return;
    
    auto res = it->second.lock();
    if (!res)
        return;

    auto* texRes = static_cast<TextureResource*>(res.get());
    texRes->MarkReady();
}