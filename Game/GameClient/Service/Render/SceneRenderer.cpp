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

SceneView& SceneRenderer::AcquireView(const ViewContext& context)
{
	Assert(context.identity.IsValid());

	SceneView*& slot = m_viewMap[context.identity]; // 없으면 nullptr로 기본 생성
	if (!slot)
	{
		m_viewPool.emplace_back(
			m_repositories, 
			m_uiQuad, 
			m_defaultMaterial,
			m_defaultBrush);
		slot = &m_viewPool.back();
	}

	slot->Reset(context);
	m_activeViews.push_back(slot);
	return *slot;
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
	frameData.views.reserve(m_activeViews.size());

	for (SceneView* view : m_activeViews)
	{
		if (view->IsEmpty())
			continue;
		frameData.views.push_back(view->TakeData());
	}

	m_renderFrame->SubmitFrame(std::move(frameData));

	m_pendingLight = {};
	m_shadowCasters.clear();
	m_activeViews.clear(); // 이번 프레임 기록만 비움.
}