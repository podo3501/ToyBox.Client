#include "pch.h"
#include "PbrMaterialResource.h"
#include "Resource/Texture/DefaultTextureType.h"

PbrMaterialResource::~PbrMaterialResource() = default;
PbrMaterialResource::PbrMaterialResource(const MaterialDesc& desc) :
    SurfaceMaterialResource{ static_cast<uint32_t>(PbrTextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::Surface);
    m_desc = static_cast<const PbrMaterialDesc&>(desc);
}

std::vector<DefaultTextureBinding> PbrMaterialResource::GetDefaultTextureBindings() const
{
    return {
        { Core::ToIndex(PbrTextureSlot::Albedo), DefaultTextureType::White },
        { Core::ToIndex(PbrTextureSlot::Normal), DefaultTextureType::FlatNormal },
        { Core::ToIndex(PbrTextureSlot::ARM), DefaultTextureType::Orange }
    };
}