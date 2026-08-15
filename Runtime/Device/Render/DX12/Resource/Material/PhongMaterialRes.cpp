#include "pch.h"
#include "PhongMaterialRes.h"
#include "Resource/Texture/BuiltinTextureType.h"

PhongMaterialRes::~PhongMaterialRes() = default;
PhongMaterialRes::PhongMaterialRes(const MaterialDesc& desc) :
    SurfaceMaterialResource{ static_cast<uint32_t>(PhongTextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::Surface);
    m_desc = static_cast<const PhongMaterialDe&>(desc);
}

std::vector<BuiltinTextureBinding> PhongMaterialRes::GetBuiltinTextureBindings() const
{
    return {
        { Core::ToIndex(PhongTextureSlot::Albedo), BuiltinTextureType::White },
        { Core::ToIndex(PhongTextureSlot::Normal), BuiltinTextureType::FlatNormal }
    };
}