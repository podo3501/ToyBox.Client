#include "pch.h"
#include "SceneRenderer.h"
#include "IRenderFrame.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Mesh/MeshRepository.h"
#include "Builtin/BuiltinMeshes.h"
#include "Builtin/BuiltinMaterials.h"

struct ResolvedDrawData
{
	std::shared_ptr<IMeshResource> meshRes;
	std::shared_ptr<IMaterialResource> matRes;
};

namespace cm = Core::Math;

SceneRenderer::~SceneRenderer() = default;
SceneRenderer::SceneRenderer(IRenderFrame* renderFrame, MeshRepository* meshRepository, MaterialRepository* matRepository) :
	m_renderFrame{ renderFrame },
	m_meshRepository{ meshRepository },
	m_matRepository{ matRepository }
{
	m_uiQuad = CreateBuiltinUIQuad(m_meshRepository);
	m_defaultMaterials = CreateBuiltinMaterials(m_matRepository);
}

void SceneRenderer::DrawSurface(MeshHandle hM, MaterialHandle hMtl, const cm::Matrix& world)
{
	if (!hMtl)
		hMtl = GetDefaultMaterial(MaterialDomain::Surface);

	auto data = ResolveResources(hM, hMtl);
	if (!data) return;

	m_renderFrame->DrawSurface(data->meshRes, data->matRes, world);
}

void SceneRenderer::DrawDebugSurface(MeshHandle hM, MaterialHandle hMtl, const cm::Matrix& world)
{
	if (!hMtl)
		hMtl = GetDefaultMaterial(MaterialDomain::DebugSurface);

	auto data = ResolveResources(hM, hMtl);
	if (!data) return;

	m_renderFrame->DrawSurface(data->meshRes, data->matRes, world);
}

void SceneRenderer::DrawUI(MaterialHandle hMtl, const Rect& dest, const Rect* source)
{
	if (!hMtl)
		hMtl = GetDefaultMaterial(MaterialDomain::UserInterface);

	auto data = ResolveResources(m_uiQuad, hMtl);
	if (!data) return;

	float width = static_cast<float>(dest.width);
	float height = static_cast<float>(dest.height);

	float centerX = dest.x + width * 0.5f;
	float centerY = dest.y + height * 0.5f;

	cm::Matrix translation = cm::Matrix::Translation(centerX, centerY, 0.0f);
	cm::Matrix scale = cm::Matrix::Scale(width, height, 1.0f);
	cm::Matrix world = scale * translation;
	
	if (source) // 텍스쳐의 부분을 가지고 올때 사용함. 
	{
		// float u0 = source->x / textureWidth;
		// float v0 = source->y / textureHeight;
		// float u1 = (source->x + source->w) / textureWidth;
		// float v1 = (source->y + source->h) / textureHeight;
		//
		// Vector2 uvScale  = { u1 - u0, v1 - v0 };
		// Vector2 uvOffset = { u0, v0 };
		//
		// matRes->SetUVTransform(uvScale, uvOffset);
	}

	m_renderFrame->DrawUI(data->meshRes, data->matRes, world);
}

std::optional<ResolvedDrawData> SceneRenderer::ResolveResources(MeshHandle hM, MaterialHandle hMtl)
{
	auto mesh = m_meshRepository->Get(hM);
	if (!mesh || mesh->state != LoadState::Ready)
		return std::nullopt;

	std::shared_ptr<IMaterialResource> matRes;

	auto material = m_matRepository->Get(hMtl);
	if (!material || material->state != LoadState::Ready)
		return std::nullopt;

	return ResolvedDrawData{ mesh->meshRes, material->matRes };
}

void SceneRenderer::SetFrameData(const FrameData& frameData)
{
	m_renderFrame->SetFrameData(frameData);
}

MaterialHandle SceneRenderer::GetDefaultMaterial(MaterialDomain matDomain) const
{
	return m_defaultMaterials[static_cast<size_t>(matDomain)];
}
