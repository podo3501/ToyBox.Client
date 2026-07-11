#include "pch.h"
#include "PbrMaterialResource.h"
#include "Resource/Texture/BuiltinTextureType.h"

PbrMaterialResource::~PbrMaterialResource() = default;
PbrMaterialResource::PbrMaterialResource(const MaterialDesc& desc) :
    SurfaceMaterialResource{ static_cast<uint32_t>(PbrTextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::Surface);
    m_desc = static_cast<const PbrMaterialDesc&>(desc);
}

std::vector<BuiltinTextureBinding> PbrMaterialResource::GetBuiltinTextureBindings() const
{
    return {
        { Core::ToIndex(PbrTextureSlot::Albedo), BuiltinTextureType::White },
        { Core::ToIndex(PbrTextureSlot::Normal), BuiltinTextureType::FlatNormal },
        { Core::ToIndex(PbrTextureSlot::ARM), BuiltinTextureType::DefaultARM }
    };
}