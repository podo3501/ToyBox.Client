#include "pch.h"
#include "MeshMaterialResource.h"
#include "TextureResource.h"

MeshMaterialResource::~MeshMaterialResource() = default;
MeshMaterialResource::MeshMaterialResource()
{
    m_texResources.resize(static_cast<size_t>(MeshTextureSlot::Count));
}

bool MeshMaterialResource::IsTextureReady() const noexcept
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

void MeshMaterialResource::SetTexture(TextureSlot texSlot, std::shared_ptr<ITextureResource> texRes) noexcept
{
    Assert(texSlot < m_texResources.size());
    m_texResources[texSlot] = std::move(texRes);
}

std::vector<std::shared_ptr<ITextureResource>> MeshMaterialResource::GetTextures() const noexcept
{ 
    return m_texResources; 
}

std::vector<UINT> MeshMaterialResource::GetTextureIndices() const noexcept
{
    std::vector<UINT> indices;

    for (auto& tex : m_texResources)
    {
        auto t = std::static_pointer_cast<TextureResource>(tex);
        indices.push_back(t->GetHeapIndex());
    }

    return indices;
}