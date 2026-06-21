#include "pch.h"
#include "UIMaterialResource.h"
#include "Resource/Texture/TextureResource.h"
#include "Resource/Texture/DefaultTextureType.h"

UIMaterialResource::~UIMaterialResource() = default;
UIMaterialResource::UIMaterialResource(const MaterialDesc& desc) :
    MaterialResource{ static_cast<uint32_t>(UITextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::UserInterface);
    m_desc = static_cast<const UIMaterialDesc&>(desc);
}

std::vector<DefaultTextureBinding> UIMaterialResource::GetDefaultTextureBindings() const
{
    return {
        { Core::ToIndex(UITextureSlot::Normal), DefaultTextureType::White }
    };
}