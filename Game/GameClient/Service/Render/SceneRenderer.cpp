#include "pch.h"
#include "SceneRenderer.h"
#include "IRenderFrame.h"
#include "Repository/Font/FontRepository.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Mesh/MeshRepository.h"
#include "Builtin/BuiltinMeshes.h"
#include "Builtin/BuiltinMaterials.h"

struct ResolvedEntries
{
	const MeshEntry* mesh{ nullptr };
	const MaterialEntry* material{ nullptr };
};

struct ResolvedDrawData
{
	std::shared_ptr<IMeshResource> meshRes;
	std::shared_ptr<IMaterialResource> matRes;
};

namespace cm = Core::Math;

SceneRenderer::~SceneRenderer() = default;
SceneRenderer::SceneRenderer(
	IRenderFrame* renderFrame, 
	FontRepository* fontRepository,
	MeshRepository* meshRepository, 
	MaterialRepository* matRepository) :
	m_renderFrame{ renderFrame },
	m_fontRepository{ fontRepository },
	m_meshRepository{ meshRepository },
	m_matRepository{ matRepository }
{
	m_uiQuad = CreateBuiltinUIQuad(m_meshRepository);
	m_defaultMaterials = CreateBuiltinMaterials(m_matRepository);
}

void SceneRenderer::DrawText(FontHandle hF, std::string_view text, const Vector2& pos)
{
	//if (!hF)
		//hF = GetDefaultMaterial(MaterialDomain::Surface);

	auto font = m_fontRepository->Get(hF);
	if (!font || font->state != LoadState::Ready)
		return;

	//m_renderFrame->DrawText(font->fontRes, text, pos);
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

	cm::Matrix scale = cm::Matrix::Scale(width, height, 1.0f);
	cm::Matrix translation = cm::Matrix::Translation(static_cast<float>(dest.x), static_cast<float>(dest.y), 0.0f);
	cm::Matrix world = scale * translation;

	m_renderFrame->DrawUI(data->meshRes, data->matRes, world, source);
}

std::optional<ResolvedDrawData> SceneRenderer::ResolveResources(MeshHandle hM, MaterialHandle hMtl)
{
	auto mesh = m_meshRepository->Get(hM);
	if (!mesh || mesh->state != LoadState::Ready)
		return std::nullopt;

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
