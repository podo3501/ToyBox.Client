#pragma once
#include "Resource/Texture/TextureCubeResource.h"
#include "Graph/RGTypes.h"

class TextureCubeRegistry
{
public:
    void Register(RGResourceID resID, std::shared_ptr<TextureCubeResource> resource);
    TextureCubeResource* GetTextureCubeResource(RGResourceID resID);
    void FinalizeTextureCube(RGResourceID resID);

private:
    std::unordered_map<RGResourceID, std::weak_ptr<TextureCubeResource>> m_textures;
};