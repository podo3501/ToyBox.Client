#pragma once
#include "Resource/Texture/TextureResource.h"
#include "Graph/RGTypes.h"

class TextureRegistry
{
public:
    void Register(RGResourceID resID, std::shared_ptr<TextureResource> resource);
    TextureResource* GetTextureResource(RGResourceID resID);
    void FinalizeTexture(RGResourceID resID);

private:
    std::unordered_map<RGResourceID, std::weak_ptr<TextureResource>> m_textures;
};