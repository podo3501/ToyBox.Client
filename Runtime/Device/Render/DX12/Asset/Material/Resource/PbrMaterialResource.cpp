#include "pch.h"
#include "PbrMaterialResource.h"
#include "Asset/Texture/TextureSystem.h"

PbrMaterialResource::~PbrMaterialResource() = default;
PbrMaterialResource::PbrMaterialResource(const MaterialDesc& desc) :
    SurfaceMaterialResource{ static_cast<uint32_t>(PbrTextureSlot::Count) }
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