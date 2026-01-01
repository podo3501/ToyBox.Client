#pragma once
#include "Shared/Foundation/ManagerBase.h"

class Scene;
namespace DX { class StepTimer; }
class SceneManager : private ManagerBase
{
public:
	void Transition(unique_ptr<Scene> newScene);
	void Update(const DX::StepTimer& timer);

	static unique_ptr<SceneManager> Create();

protected:
	SceneManager();

private:
	void DoTransition();

	unique_ptr<Scene> m_currentScene;
	unique_ptr<Scene> m_pendingScene;
};