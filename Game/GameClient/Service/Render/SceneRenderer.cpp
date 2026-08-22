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
	SceneView* view;
	if (m_activeViewCount < m_viewPool.size())
		view = &m_viewPool[m_activeViewCount]; // 이미 만들어진 view 재사용
	else
	{
		m_viewPool.emplace_back(m_repositories, m_uiQuad, m_defaultMaterial, m_defaultBrush); // 풀이 부족할 때만 새로 생성 (이후 프레임부턴 대부분 여기 안 옴)
		view = &m_viewPool.back();
	}

	view->Reset(context);
	++m_activeViewCount;
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

	frameData.views.reserve(m_activeViewCount);
	for (size_t i = 0; i < m_activeViewCount; ++i)
	{
		auto& view = m_viewPool[i];
		if (view.IsEmpty())
			continue;
		frameData.views.push_back(view.TakeData());
	}

	m_renderFrame->SubmitFrame(std::move(frameData));

	m_pendingLight = {};
	m_shadowCasters.clear();
	m_activeViewCount = 0; // pool 원소들은 삭제 안 하고 그대로 둠. 다음 CreateView가 다시 재사용
}