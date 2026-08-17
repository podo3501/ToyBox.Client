#include "pch.h"
#include "SceneRenderer.h"
#include "IRenderFrame.h"
#include "Repository/Container/RepositoryContainer.h"
#include "Repository/Container/RepositoryTypeTraits.h"
#include "Repository/ResourceRepositories.h"
#include "Builtin/BuiltinMeshes.h"
#include "Builtin/BuiltinMaterials.h"
#include "Builtin/BuiltinBrush.h"

SceneRenderer::~SceneRenderer() = default;
SceneRenderer::SceneRenderer(
	IRenderFrame* renderFrame,
	RepositoryContainer& repositories) :
	m_renderFrame{ renderFrame },
	m_repositories{ repositories }
{
	auto& meshRepository = m_repositories.Get<MeshRepository>();
	m_uiQuad = CreateBuiltinUIQuad(meshRepository);

	auto& materialRepository = m_repositories.Get<MaterialRepository>();
	m_defaultMaterial = CreateBuiltinMaterials(materialRepository);

	auto& brushRepository = m_repositories.Get<BrushRepository>();
	m_defaultBrush = CreateBuiltinBrush(brushRepository);
}

void SceneRenderer::BeginView(const ViewContext& view)
{
	m_renderFrame->BeginView(view);
}

void SceneRenderer::EndView()
{
	m_renderFrame->EndView();
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
	auto& fontRepository = m_repositories.Get<FontRepository>();
	auto fontRes = fontRepository.GetIfReady(hF);
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

void SceneRenderer::DrawSurface(
	MeshHandle hM,
	MaterialHandle hMtl,
	const Core::Matrix& world)
{
	DrawSurfaceInternal(hM, hMtl, std::nullopt, world);
}

void SceneRenderer::DrawWithShaderOverride(
	MeshHandle hM,
	MaterialHandle hMtl,
	ShaderID shaderID,
	const Core::Matrix& world)
{
	DrawSurfaceInternal(hM, hMtl, shaderID, world);
}

void SceneRenderer::DrawSurfaceInternal(
	MeshHandle hM,
	MaterialHandle hMtl,
	std::optional<ShaderID> shaderOverride,
	const Core::Matrix& world)
{
	if (!hMtl)
		hMtl = m_defaultMaterial;

	auto& meshRepository = m_repositories.Get<MeshRepository>();
	auto meshRes = meshRepository.GetIfReady(hM);
	if (!meshRes)
		return;

	auto& materialRepository = m_repositories.Get<MaterialRepository>();
	auto materialRes = materialRepository.GetIfReady(hMtl);
	if (!materialRes)
		return;

	m_renderFrame->DrawSurface(
		meshRes,
		materialRes,
		shaderOverride,
		world);
}

void SceneRenderer::DrawDebugSurface(DebugMeshHandle hDM, DebugMaterialHandle hDMtl, const Core::Matrix& world)
{
	auto& debugMeshRepository = m_repositories.Get<DebugMeshRepository>();
	auto meshRes = debugMeshRepository.GetIfReady(hDM);
	if (!meshRes)
		return;

	auto& debugMaterialRepository = m_repositories.Get<DebugMaterialRepository>();
	auto materialRes = debugMaterialRepository.GetIfReady(hDMtl);
	if (!materialRes)
		return;

	m_renderFrame->DrawDebugSurface(meshRes, materialRes, world);
}

void SceneRenderer::DrawUI(BrushHandle bh, const Rect& dest, const Rect* source)
{
	if (!bh)
		bh = m_defaultBrush;

	auto& meshRepository = m_repositories.Get<MeshRepository>();
	auto meshRes = meshRepository.GetIfReady(m_uiQuad);
	if (!meshRes)
		return;

	auto& brushRepository = m_repositories.Get<BrushRepository>();
	auto brushRes = brushRepository.GetIfReady(bh);
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

	auto& envRepository = m_repositories.Get<EnvironmentRepository>();
	auto envRes = envRepository.GetIfReady(hEnv);
	if (!envRes)
		return;

	m_renderFrame->DrawEnvironment(envRes);
}

void SceneRenderer::SetFrameData(const FrameData& frameData)
{
	m_renderFrame->SetFrameData(frameData);
}