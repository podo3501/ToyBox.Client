#include "pch.h"
#include "PbrMaterialRes.h"
#include "Resource/Texture/BuiltinTextureType.h"

PbrMaterialRes::~PbrMaterialRes() = default;
PbrMaterialRes::PbrMaterialRes(const MaterialDesc& desc) :
    SurfaceMaterialResource{ static_cast<uint32_t>(PbrTextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::Surface);
    m_desc = static_cast<const PbrMaterialDe&>(desc);
}

std::vector<BuiltinTextureBinding> PbrMaterialRes::GetBuiltinTextureBindings() const
{
    return {
        { Core::ToIndex(PbrTextureSlot::Albedo), BuiltinTextureType::White },
        { Core::ToIndex(PbrTextureSlot::Normal), BuiltinTextureType::FlatNormal },
        { Core::ToIndex(PbrTextureSlot::ARM), BuiltinTextureType::DefaultARM }
    };
}