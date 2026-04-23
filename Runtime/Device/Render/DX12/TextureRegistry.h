#pragma once
#include "TextureResource.h"
#include "DescriptorAllocation.h"
#include "RGTypes.h"

class TextureRegistry
{
public:
    void Register(uint32_t id, TextureResource* res);
    void FinalizeTexture(uint32_t id, ComPtr<ID3D12Resource> resource, DescriptorAllocation alloc);
    TextureResource* Get(uint32_t id) { return m_textures[id]; }

private:
    std::unordered_map<uint32_t, TextureResource*> m_textures;
};
