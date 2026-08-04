#include "pch.h"
#include "SceneRenderer.h"
#include "IRenderFrame.h"
#include "Repository/Font/FontRepository.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Mesh/MeshRepository.h"
#include "Repository/Environment/EnvironmentRepository.h"
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

SceneRenderer::~SceneRenderer() = default;
SceneRenderer::SceneRenderer(
	IRenderFrame* renderFrame, 
	FontRepository* fontRepository,
	MeshRepository* meshRepository, 
	MaterialRepository* matRepository,
	EnvironmentRepository* envRepository) :
	m_renderFrame{ renderFrame },
	m_fontRepository{ fontRepository },
	m_meshRepository{ meshRepository },
	m_matRepository{ matRepository },
	m_envRepository{ envRepository }
{
	m_uiQuad = CreateBuiltinUIQuad(m_meshRepository);
	m_defaultMaterials = CreateBuiltinMaterials(m_matRepository);
}

void SceneRenderer::DrawText(
	FontHandle hF, 
	TextRenderMode mode,
	std::string_view text, 
	uint32_t size, 
	const Rect& bounds, 
	const TextLayout& layout,
	const TextStyle& style)
{
	TextSpan span{ text, style };
	DrawText(hF, mode, std::span{ &span, 1 }, size, bounds, layout);
}

void SceneRenderer::DrawText(
	FontHandle hF, 
	TextRenderMode mode,
	std::span<const TextSpan> spans, 
	uint32_t size, 
	const Rect& bounds,
	const TextLayout& layout)
{
	auto font = m_fontRepository->Get(hF);
	if (!font || font->state != LoadState::Ready)
		return;

	if (mode == TextRenderMode::Bitmap)
	{
		for (auto& span : spans)
		{
			auto& style = span.style;
			//비트맵에는 이 기능들이 없다. 만약 Bitmap에 기능을 추가하면 여기서 assert를 제거.
			//아예 style을 따로 갈수도 있지만, 그러기에는 구현 비용이 크다. 그리고 bitmap이라고 이 기능이 구현이 안되는것도 아니다.
			Assert(!style.outline.has_value()); 
			Assert(!style.shadow.has_value());
			Assert(!style.gradient.has_value());
			Assert(!style.glow.has_value());
		}
	}

	m_renderFrame->DrawText(font->fontRes, mode, spans, size, bounds, layout);
}

void SceneRenderer::DrawSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Matrix& world)
{
	if (!hMtl)
		hMtl = GetDefaultMaterial(MaterialDomain::Surface);

	auto data = ResolveResources(hM, hMtl);
	if (!data) return;

	m_renderFrame->DrawSurface(data->meshRes, data->matRes, world);
}

void SceneRenderer::DrawDebugSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Matrix& world)
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

	float width = dest.width;
	float height = dest.height;

	Core::Matrix scale = Core::Matrix::Scale(width, height, 1.0f);
	Core::Matrix translation = Core::Matrix::Translation(dest.x, dest.y, 0.0f);
	Core::Matrix world = scale * translation;

	m_renderFrame->DrawUI(data->meshRes, data->matRes, world, source);
}

void SceneRenderer::DrawEnvironment(EnvironmentHandle hEnv)
{
	if (!hEnv) return;

	auto entry = m_envRepository->Get(hEnv);
	if (!entry || entry->state != LoadState::Ready)
		return;

	m_renderFrame->DrawEnvironment(entry->envRes);
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
