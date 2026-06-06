#include "pch.h"
#include "PbrMaterialResource.h"
#include "../TextureSystem.h"

PbrMaterialResource::~PbrMaterialResource() = default;
PbrMaterialResource::PbrMaterialResource(const MaterialDesc& desc) :
    MaterialResource{ static_cast<size_t>(PbrTextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::Surface);
    m_desc = static_cast<const PbrMaterialDesc&>(desc);
}

std::vector<DefaultTextureType> PbrMaterialResource::GetRequiredDefaultTextures() const
{
    return {
        DefaultTextureType::White,      // PbrTextureSlot::Albedo
        DefaultTextureType::FlatNormal, // PbrTextureSlot::Normal
        DefaultTextureType::Orange      // PbrTextureSlot::ARM
    };
}