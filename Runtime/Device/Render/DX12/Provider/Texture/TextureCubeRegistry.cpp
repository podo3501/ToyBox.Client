#include "pch.h"
#include "TextureCubeRegistry.h"

void TextureCubeRegistry::Register(RGResourceID resID, std::shared_ptr<TextureCubeResource> resource)
{
    m_textures[resID] = resource;
}

TextureCubeResource* TextureCubeRegistry::GetTextureCubeResource(RGResourceID resID)
{
    auto it = m_textures.find(resID);
    if (it == m_textures.end()) return nullptr;

    auto res = it->second.lock();
    if (!res)
        return nullptr;

    return static_cast<TextureCubeResource*>(res.get());
}

void TextureCubeRegistry::FinalizeTextureCube(RGResourceID resID)
{
    auto it = m_textures.find(resID);
    if (it == m_textures.end()) return;

    auto res = it->second.lock();
    if (!res)
        return;

    auto* texRes = static_cast<TextureCubeResource*>(res.get());
    texRes->MarkReady();
}