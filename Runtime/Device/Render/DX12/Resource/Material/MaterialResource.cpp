#include "pch.h"
#include "MaterialResource.h"
#include "Resource/Texture/TextureResource.h"

MaterialResource::MaterialResource(uint32_t texSlotCount)
{
    m_texResources.resize(texSlotCount);
}

MaterialDomain MaterialResource::GetDomain() const noexcept
{
    return GetMaterialDesc().domain;
}

const PipelineState& MaterialResource::GetPipelineState() const
{
	return GetMaterialDesc().pipelineState;
}

bool MaterialResource::IsTextureReady() const noexcept
{
    const auto& desc = GetMaterialDesc();
    const size_t count = desc.textures.size();
    Assert(m_texResources.size() >= count);

    for (size_t i = 0; i < count; ++i)
    {
        const auto& tex = m_texResources[i];
        if (!tex || !tex->IsReady())
            return false;
    }
    return true;
}

void MaterialResource::SetTexture(TextureSlot texSlot, std::shared_ptr<TextureResource> texRes) noexcept
{
    Assert(texSlot < m_texResources.size());
    m_texResources[texSlot] = texRes;
}

TextureResource* MaterialResource::GetTexture(TextureSlot texSlot) const noexcept
{
    Assert(texSlot < m_texResources.size());
    return m_texResources[texSlot].get();
}

std::vector<UINT> MaterialResource::GetTextureIndices() const noexcept
{
    std::vector<UINT> indices;
    indices.reserve(m_texResources.size());

    for (auto& tex : m_texResources)
    {
        Assert(tex);
        auto t = std::static_pointer_cast<TextureResource>(tex);
        indices.push_back(t->GetHeapIndex());
    }

    return indices;
}