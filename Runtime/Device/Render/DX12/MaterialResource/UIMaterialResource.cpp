#include "pch.h"
#include "UIMaterialResource.h"
#include "../TextureResource.h"
#include "../TextureSystem.h"

UIMaterialResource::~UIMaterialResource() = default;
UIMaterialResource::UIMaterialResource(const MaterialDesc& desc) :
    MaterialResource{ static_cast<uint32_t>(UITextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::UserInterface);
    m_desc = static_cast<const UIMaterialDesc&>(desc);
}

std::vector<DefaultTextureType> UIMaterialResource::GetRequiredDefaultTextures() const
{
    return {
        DefaultTextureType::White       // UITextureSlot::Normal (UI 일반 이미지)
    };
}