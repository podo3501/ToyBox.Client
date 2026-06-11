#pragma once
#include "TextureResource.h"
#include "Graph/RGTypes.h"

class TextureRegistry
{
public:
    void Register(uint32_t id, std::shared_ptr<ITextureResource> resource);
    TextureResource* GetTextureResource(uint32_t id);
    void FinalizeTexture(uint32_t id);

private:
    std::unordered_map<uint32_t, std::weak_ptr<ITextureResource>> m_textures;
};