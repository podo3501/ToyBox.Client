#pragma once
#include "ISceneManager.h"

class Scene;
class SceneManager : public ISceneManager
{
public:
	~SceneManager();
	SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager const&) = delete;

	virtual void Transition(unique_ptr<Scene> newScene) override;
	virtual void Update(const DX::StepTimer& timer) override;

private:
	void DoTransition();

	unique_ptr<Scene> m_currentScene;
	unique_ptr<Scene> m_pendingScene;
};