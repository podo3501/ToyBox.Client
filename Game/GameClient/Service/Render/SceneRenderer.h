#pragma once
#include "SceneView.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "Definition/View/ViewID.h"
#include "Definition/View/DrawShadowCasterItem.h"

struct SceneFrameData;
class RepositoryContainer;

class SceneRenderer
{
public:
	~SceneRenderer();
	SceneRenderer() = delete;
	explicit SceneRenderer(RepositoryContainer& repositories);

	SceneView& AcquireView(
		const ViewContext& context,
		const Camera& camera,
		const Size& screenSize);

	void SetLight(const DirectionalLightData& light);
	void DrawShadowCaster(MeshHandle hM, const Core::Matrix& world);
	SceneFrameData Flush();

private:
	RepositoryContainer& m_repositories;

	//default(built in)
	MeshHandle m_uiQuad{};
	MaterialHandle m_defaultMaterial;
	BrushHandle m_defaultBrush;

	DirectionalLightData m_pendingLight;
	std::vector<DrawShadowCasterItem> m_shadowCasters;
	std::array<std::unique_ptr<SceneView>, Core::EnumSize<ViewID>> m_views;
};