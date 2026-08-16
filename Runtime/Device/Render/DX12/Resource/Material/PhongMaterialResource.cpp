#include "pch.h"
#include "PhongMaterialResource.h"
#include "../Texture/TextureResource.h"

PhongMaterialResource::~PhongMaterialResource() = default;
PhongMaterialResource::PhongMaterialResource() :
	MaterialResource{
        MaterialType::Phong,
		PipelineLibrary::Get(
			RegistryShader::Phong,
			RasterPreset::Default,
			PrimitiveTopologyType::Triangle)
	}
{}

bool PhongMaterialResource::IsDependencyReady() const noexcept
{
    // albedo는 필수
    if (!m_albedo || !m_albedo->IsReady())
        return false;

    // normal은 선택
    if (m_normal && !m_normal->IsReady())
        return false;

    return true;
}