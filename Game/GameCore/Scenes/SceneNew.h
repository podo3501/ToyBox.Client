#pragma once

class SceneState;
class SceneLayer;
class SceneNew
{
public:
	~SceneNew();
	SceneNew() = delete;
	SceneNew(unique_ptr<SceneState> sceneState, unique_ptr<SceneLayer> sceneLayer);

private:
	unique_ptr<SceneLayer> m_sceneLayer;
	unique_ptr<SceneState> m_sceneState;
};
