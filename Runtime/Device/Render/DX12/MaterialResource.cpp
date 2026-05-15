#include "pch.h"
#include "MaterialResource.h"
#include "TextureResource.h"

MaterialResource::~MaterialResource() = default;
MaterialResource::MaterialResource() = default;

bool MaterialResource::IsReady() const noexcept
{
    if (!m_texRes)
        return false;

    return m_texRes->IsReady();
}

void MaterialResource::SetTexture(std::shared_ptr<ITextureResource> texRes)
{
    m_texRes = texRes;
}

DescriptorAllocation& MaterialResource::GetTextureSRV()
{
    auto texRes = static_cast<TextureResource*>(m_texRes.get());
    return texRes->GetSrv();
}
