#pragma once
#include "SceneView.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "Definition/View/DrawShadowCasterItem.h"

struct IRenderFrame;
struct FrameData;
class RepositoryContainer;

class SceneRenderer
{
public:
	~SceneRenderer();
	SceneRenderer() = delete;
	SceneRenderer(
		IRenderFrame* renderFrame, 
		RepositoryContainer& repositories);

	SceneView& AcquireView(
		const ViewContext& context,
		const Camera& camera,
		const Size& screenSize);

	void SetLight(const DirectionalLightData& light);
	void DrawShadowCaster(MeshHandle hM, const Core::Matrix& world);
	void Flush();

private:
	IRenderFrame* m_renderFrame{ nullptr };
	RepositoryContainer& m_repositories;

	//default(built in)
	MeshHandle m_uiQuad{};
	MaterialHandle m_defaultMaterial;
	BrushHandle m_defaultBrush;

	DirectionalLightData m_pendingLight;
	std::vector<DrawShadowCasterItem> m_shadowCasters;

	std::deque<SceneView> m_viewPool;                          // 한 번 만들어지면 삭제 안 함
	std::unordered_map<ViewIdentity, SceneView*, ViewIdentityHash> m_viewMap;
	std::vector<SceneView*> m_activeViews;                     // 이번 프레임에 사용된 view들
};