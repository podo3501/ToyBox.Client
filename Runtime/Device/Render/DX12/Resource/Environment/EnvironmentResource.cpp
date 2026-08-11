#include "pch.h"
#include "EnvironmentResource.h"
#include "../Texture/TextureCubeResource.h"

EnvironmentResource::~EnvironmentResource() = default;
EnvironmentResource::EnvironmentResource() = default;

bool EnvironmentResource::IsDependencyReady() const noexcept
{
	// 텍스처 업로드(비동기) 완료 여부. SH는 CPU 데이터 복사라 즉시 완료됨.
	return m_skybox && m_reflection && m_skybox->IsReady() && m_reflection->IsReady();
}