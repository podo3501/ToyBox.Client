#include "pch.h"
#include "MaterialResource.h"
#include "../TextureResource.h"

MaterialResource::MaterialResource(uint32_t texSlotCount)
{
    m_texResources.resize(texSlotCount);
}

MaterialDomain MaterialResource::GetDomain() const noexcept
{
	auto& desc = GetMaterialDesc();
	return desc.domain;
}

const PipelineState& MaterialResource::GetPipelineState() const
{
	auto& desc = GetMaterialDesc();
	return desc.pipelineState;
}

bool MaterialResource::IsTextureReady() const noexcept
{
    auto& desc = GetMaterialDesc();
    const size_t count = desc.textures.size();
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

void MaterialResource::SetTexture(TextureSlot texSlot, std::shared_ptr<ITextureResource> texRes) noexcept
{
    Assert(texSlot < m_texResources.size());
    m_texResources[texSlot] = texRes;
}

std::vector<std::shared_ptr<ITextureResource>> MaterialResource::GetTextures() const noexcept
{
    return m_texResources;
}

std::vector<UINT> MaterialResource::GetTextureIndices() const noexcept
{
    std::vector<UINT> indices;

    for (auto& tex : m_texResources)
    {
        auto t = std::static_pointer_cast<TextureResource>(tex);
        indices.push_back(t->GetHeapIndex());
    }

    return indices;
}