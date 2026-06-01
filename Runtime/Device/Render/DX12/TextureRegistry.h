#pragma once
#include "TextureResource.h"
#include "RGTypes.h"

class TextureRegistry
{
public:
    void Register(uint32_t id, std::shared_ptr<ITextureResource> resource);
    TextureResource* GetTextureResource(uint32_t id);
    void FinalizeTexture(uint32_t id);
    //void FinalizeTexture(uint32_t id, ComPtr<ID3D12Resource> resource, UINT heapIndex);

private:
    std::unordered_map<uint32_t, std::weak_ptr<ITextureResource>> m_textures;
};
