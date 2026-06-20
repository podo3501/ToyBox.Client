#include "pch.h"
#include "PhongMaterialResource.h"
#include "Resource/Texture/DefaultTextureType.h"

PhongMaterialResource::~PhongMaterialResource() = default;
PhongMaterialResource::PhongMaterialResource(const MaterialDesc& desc) :
    SurfaceMaterialResource{ static_cast<uint32_t>(PhongTextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::Surface);
    m_desc = static_cast<const PhongMaterialDesc&>(desc);
}

std::vector<DefaultTextureType> PhongMaterialResource::GetRequiredDefaultTextures() const
{
    return {
        DefaultTextureType::White,      // PhongTextureSlot::Albedo
        DefaultTextureType::FlatNormal // PhongTextureSlot::Normal
    };
}