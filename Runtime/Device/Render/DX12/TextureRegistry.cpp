#include "pch.h"
#include "TextureRegistry.h"

void TextureRegistry::Register(uint32_t id, std::shared_ptr<ITextureResource> resource)
{
    m_textures[id] = resource;
}

void TextureRegistry::FinalizeTexture(uint32_t id, ComPtr<ID3D12Resource> resource, DescriptorAllocation alloc)
{
    auto it = m_textures.find(id);
    if (it == m_textures.end()) return;
    
    auto res = it->second.lock();
    if (!res)
        return;

    auto* texRes = static_cast<TextureResource*>(res.get());
    texRes->SetResource(std::move(resource));
    texRes->SetSRV(std::move(alloc));
    texRes->MarkReady();
}