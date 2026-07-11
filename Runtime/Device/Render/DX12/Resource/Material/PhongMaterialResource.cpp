#include "pch.h"
#include "PhongMaterialResource.h"
#include "Resource/Texture/BuiltinTextureType.h"

PhongMaterialResource::~PhongMaterialResource() = default;
PhongMaterialResource::PhongMaterialResource(const MaterialDesc& desc) :
    SurfaceMaterialResource{ static_cast<uint32_t>(PhongTextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::Surface);
    m_desc = static_cast<const PhongMaterialDesc&>(desc);
}

std::vector<BuiltinTextureBinding> PhongMaterialResource::GetBuiltinTextureBindings() const
{
    return {
        { Core::ToIndex(PhongTextureSlot::Albedo), BuiltinTextureType::White },
        { Core::ToIndex(PhongTextureSlot::Normal), BuiltinTextureType::FlatNormal }
    };
}