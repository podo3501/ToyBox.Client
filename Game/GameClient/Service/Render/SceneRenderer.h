#pragma once
#include "SceneView.h"

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

	void Flush();

	SceneView& CreateView(const ViewContext& context);

	void SetFrameData(const FrameData& frameData);

private:
	IRenderFrame* m_renderFrame{ nullptr };
	RepositoryContainer& m_repositories;

	//default(built in)
	MeshHandle m_uiQuad{};
	MaterialHandle m_defaultMaterial;
	BrushHandle m_defaultBrush;

	std::deque<SceneView> m_viewPool;   // 한 번 만들어지면 삭제 안 함, 계속 재사용
	size_t m_activeViewCount{ 0 };      // 이번 프레임에 CreateView로 사용된 개수
};