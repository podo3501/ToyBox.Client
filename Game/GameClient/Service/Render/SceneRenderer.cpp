#include "pch.h"
#include "SceneRenderer.h"
#include "IRenderFrame.h"
#include "Repository/Material/MaterialRepository.h"
#include "Builtin/BuiltinMeshes.h"
#include "Builtin/BuiltinMaterials.h"
#include "Builtin/BuiltinBrush.h"

#include "Repository/ResourceRepositories.h"

SceneRenderer::~SceneRenderer() = default;
SceneRenderer::SceneRenderer(
	IRenderFrame* renderFrame, 
	FontRepository* fontRepository,
	MeshRepository* meshRepository, 
	DebugMeshRepository* debugMeshRepository,
	MaterialRepository* matRepository,
	BrushRepository* brushRepository,
	EnvironmentRepository* envRepository) :
	m_renderFrame{ renderFrame },
	m_fontRepository{ fontRepository },
	m_meshRepository{ meshRepository },
	m_debugMeshRepository{ debugMeshRepository },
	m_matRepository{ matRepository },
	m_brushRepository{ brushRepository },
	m_envRepository{ envRepository }
{
	m_uiQuad = CreateBuiltinUIQuad(m_meshRepository);
	m_defaultMaterials = CreateBuiltinMaterials(m_matRepository);
	m_defaultBrush = CreateBuiltinBrush(m_brushRepository);
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
	auto fontRes = m_fontRepository->GetIfReady(hF);
	if (!fontRes)
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

	m_renderFrame->DrawText(fontRes, mode, spans, size, bounds, layout);
}

void SceneRenderer::DrawSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Matrix& world)
{
	if (!hMtl)
		hMtl = GetDefaultMaterial(MaterialDomain::Surface);

	auto meshRes = m_meshRepository->GetIfReady(hM);
	if (!meshRes)
		return;

	auto material = m_matRepository->Get(hMtl);
	if (!material || material->state != LoadState::Ready)
		return;

	m_renderFrame->DrawSurface(meshRes, material->matRes, world);
}

void SceneRenderer::DrawDebugSurface(DebugMeshHandle hDM, MaterialHandle hMtl, const Core::Matrix& world)
{
	if (!hMtl)
		hMtl = GetDefaultMaterial(MaterialDomain::DebugSurface);

	auto meshRes = m_debugMeshRepository->GetIfReady(hDM);
	if (!meshRes)
		return;

	auto material = m_matRepository->Get(hMtl);
	if (!material || material->state != LoadState::Ready)
		return;

	m_renderFrame->DrawSurface(meshRes, material->matRes, world);
}

void SceneRenderer::DrawUI(BrushHandle bh, const Rect& dest, const Rect* source)
{
	if (!bh)
		bh = m_defaultBrush;

	auto meshRes = m_meshRepository->GetIfReady(m_uiQuad);
	if (!meshRes)
		return;

	auto brushRes = m_brushRepository->GetIfReady(bh);
	if (!brushRes)
		return;

	float width = dest.width;
	float height = dest.height;

	Core::Matrix scale = Core::Matrix::Scale(width, height, 1.0f);
	Core::Matrix translation = Core::Matrix::Translation(dest.x, dest.y, 0.0f);
	Core::Matrix world = scale * translation;

	m_renderFrame->DrawUI(meshRes, brushRes, world, source);
}

void SceneRenderer::DrawEnvironment(EnvironmentHandle hEnv)
{
	if (!hEnv) return;

	auto envRes = m_envRepository->GetIfReady(hEnv);
	if (!envRes)
		return;

	m_renderFrame->DrawEnvironment(envRes);
}

void SceneRenderer::SetFrameData(const FrameData& frameData)
{
	m_renderFrame->SetFrameData(frameData);
}

MaterialHandle SceneRenderer::GetDefaultMaterial(MaterialDomain matDomain) const
{
	return m_defaultMaterials[static_cast<size_t>(matDomain)];
}
