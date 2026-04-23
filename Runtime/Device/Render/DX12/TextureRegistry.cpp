#include "pch.h"
#include "TextureRegistry.h"

void TextureRegistry::Register(uint32_t id, TextureResource* res)
{
    m_textures[id] = res;
}

void TextureRegistry::FinalizeTexture(uint32_t id, ComPtr<ID3D12Resource> resource, DescriptorAllocation alloc)
{
    auto res = m_textures[id];

    res->SetResource(std::move(resource));
    res->SetSRV(std::move(alloc));
    res->MarkReady();
}