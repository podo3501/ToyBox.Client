#include "pch.h"
#include "SceneRenderer.h"
#include "Repository/Container/RepositoryContainer.h"
#include "Repository/Container/RepositoryTypeTraits.h"
#include "Builtin/BuiltinMeshes.h"
#include "Builtin/BuiltinMaterials.h"
#include "Builtin/BuiltinBrush.h"
#include "Definition/View/SceneFrameData.h"

SceneRenderer::~SceneRenderer() = default;
SceneRenderer::SceneRenderer(RepositoryContainer& repositories) :
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
	const SceneViewContext& context,
	const Camera& camera,
	const Size& screenSize)
{
	Assert(context.target.id < MaxViewCount);
	auto& slot = m_views[context.target.id];

	if (!slot)
		slot = std::make_unique<SceneView>(m_repositories, m_defaultMaterial);
	
	Assert(slot->Type() == ViewType::Scene); // 같은 ID를 OverlayView가 이미 점유했다면 문제.

	auto* view = static_cast<SceneView*>(slot.get());
	view->Reset(context, camera, screenSize);

	return *view;
}

OverlayView& SceneRenderer::AcquireView(
	const OverlayViewContext& context,
	const Camera& camera,
	const Size& screenSize)
{
	Assert(context.target.id < MaxViewCount);
	auto& slot = m_views[context.target.id];

	if (!slot)
		slot = std::make_unique<OverlayView>(m_repositories, m_uiQuad, m_defaultBrush);

	Assert(slot->Type() == ViewType::Overlay);

	auto* view = static_cast<OverlayView*>(slot.get());
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

SceneFrameData SceneRenderer::Flush()
{
	SceneFrameData frameData;
	frameData.light = std::move(m_pendingLight);
	frameData.shadowCasters = std::move(m_shadowCasters);

	for (auto& view : m_views)
	{
		if (!view || view->IsEmpty())
			continue;

		switch (view->Type())
		{
		case ViewType::Scene:
			frameData.sceneViews.push_back(static_cast<SceneView*>(view.get())->TakeData());
			break;
		case ViewType::Overlay:
			frameData.overlayViews.push_back(static_cast<OverlayView*>(view.get())->TakeData());
			break;
		default:
			Assert(false); // None 상태의 슬롯이 배열에 남아있으면 안 됨
			break;
		}
	}

	m_pendingLight = {};
	m_shadowCasters.clear();

	return frameData;
}