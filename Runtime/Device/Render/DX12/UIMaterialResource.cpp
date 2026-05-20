#include "pch.h"
#include "UIMaterialResource.h"
#include "TextureResource.h"

UIMaterialResource::~UIMaterialResource() = default;
UIMaterialResource::UIMaterialResource() = default;

bool UIMaterialResource::IsReady() const noexcept
{
    if (!m_texRes)
        return false;

    return m_texRes->IsReady();
}

void UIMaterialResource::SetTexture(std::shared_ptr<ITextureResource> texRes)
{
    m_texRes = texRes;
}

DescriptorAllocation& UIMaterialResource::GetTextureSRV()
{
    auto texRes = static_cast<TextureResource*>(m_texRes.get());
    return texRes->GetSrv();
}