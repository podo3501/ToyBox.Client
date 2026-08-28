#include "pch.h"
#include "PbrMaterialResource.h"
#include "../Texture/TextureResource.h"
#include "GameClient/Service/Render/Definition/Shader/RegistryShader.h"

PbrMaterialResource::~PbrMaterialResource() = default;
PbrMaterialResource::PbrMaterialResource() :
	MaterialResource{
        MaterialType::PBR,
		PipelineLibrary::Get(
			RegistryShader::PBR,
			RasterPreset::Default,
			PrimitiveTopologyType::Triangle)
	}
{}

bool PbrMaterialResource::IsDependencyReady() const noexcept
{
    // albedo는 필수
    if (!m_albedo || !m_albedo->IsReady())
        return false;

    // normal, arm은 선택
    if (m_normal && !m_normal->IsReady())
        return false;

    if (m_arm && !m_arm->IsReady())
        return false;

    return true;
}