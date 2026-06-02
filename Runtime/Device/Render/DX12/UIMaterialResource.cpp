#include "pch.h"
#include "UIMaterialResource.h"
#include "TextureResource.h"

UIMaterialResource::~UIMaterialResource() = default;
UIMaterialResource::UIMaterialResource(const MaterialDesc& desc)
{
    Assert(desc.type == MaterialType::UI);
    m_desc = static_cast<const UIMaterialDesc&>(desc);
}

bool UIMaterialResource::IsTextureReady() const noexcept
{
    return m_texResource->IsReady();
}

void UIMaterialResource::SetTexture(TextureSlot texSlot, std::shared_ptr<ITextureResource> texRes) noexcept
{
    Assert(texSlot == 0); //일단은 지금은 하나만 들어온다.
    m_texResource = texRes;
}

UINT UIMaterialResource::GetTextureHeapIndex() const noexcept
{
    auto t = std::static_pointer_cast<TextureResource>(m_texResource);
    return t->GetHeapIndex();
}