#pragma once
#include "TextureResource.h"
#include "Descriptor/DescriptorAllocation.h"
#include "RGTypes.h"

class TextureRegistry
{
public:
    void Register(uint32_t id, std::shared_ptr<ITextureResource> resource);
    void FinalizeTexture(uint32_t id, ComPtr<ID3D12Resource> resource, DescriptorAllocation alloc);

private:
    std::unordered_map<uint32_t, std::weak_ptr<ITextureResource>> m_textures;
};
