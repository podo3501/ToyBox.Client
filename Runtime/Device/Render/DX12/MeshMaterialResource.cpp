#include "pch.h"
#include "MeshMaterialResource.h"
#include "TextureResource.h"

MeshMaterialResource::~MeshMaterialResource() = default;
MeshMaterialResource::MeshMaterialResource()
{
    m_texResources.resize(static_cast<size_t>(TextureSlot::Count));
}

bool MeshMaterialResource::IsReady() const noexcept
{
    const size_t count = m_desc.textures.size();
    if (m_texResources.size() < count)
        return false;

    for (size_t i = 0; i < count; ++i)
    {
        const auto& tex = m_texResources[i];
        if (!tex || !tex->IsReady())
            return false;
    }
    return true;
}

void MeshMaterialResource::SetTexture(TextureSlot texSlot, std::shared_ptr<ITextureResource> texRes)
{
    const size_t slot = static_cast<size_t>(texSlot);
    Assert(slot < m_texResources.size());

    m_texResources[slot] = std::move(texRes);
}

DescriptorAllocation& MeshMaterialResource::GetTextureSRV(TextureSlot texSlot)
{
    const size_t slot = static_cast<size_t>(texSlot);
    Assert(slot < m_texResources.size());

    auto texRes = static_cast<TextureResource*>(m_texResources[slot].get());
    return texRes->GetSrv();
}
