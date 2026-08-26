#include "pch.h"
#include "SceneRenderer.h"
#include "IRenderFrame.h"
#include "Repository/Container/RepositoryContainer.h"
#include "Repository/Container/RepositoryTypeTraits.h"
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

SceneView& SceneRenderer::AcquireView(
	const ViewContext& context,
	const Camera& camera, 
	const Size& screenSize)
{
	auto& view = m_views[Core::ToIndex(context.id)]; // 없으면 nullptr로 기본 생성
	if (!view)
	{
		view = std::make_unique<SceneView>(
			m_repositories,
			m_uiQuad,
			m_defaultMaterial,
			m_defaultBrush);
	}

	view->Reset(context, camera, screenSize);
	return *view;
}

void SceneRenderer::SetLight(const DirectionalLightData& light)
{
	m_pendingLight = light; // Flush에서 SceneFrameData::light로 옮김
}

void SceneRenderer::DrawShadowCaster(MeshHandle hM, const Core::Matrix& world)
{
	auto& meshRepository = m_repositories.Get<MeshRepository>();
	auto meshRes = meshRepository.GetIfReady(hM);
	if (!meshRes)
		return;

	m_shadowCasters.push_back(DrawShadowCasterItem{ meshRes, world });
}

void SceneRenderer::Flush()
{
	SceneFrameData frameData;
	frameData.light = std::move(m_pendingLight);
	frameData.shadowCasters = std::move(m_shadowCasters);

	for (auto& view : m_views)
	{
		if (!view || view->IsEmpty())
			continue;

		frameData.views.push_back(view->TakeData());
	}

	m_renderFrame->SubmitFrame(std::move(frameData));

	m_pendingLight = {};
	m_shadowCasters.clear();
}