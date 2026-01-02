#pragma once
#include "Shared/Foundation/NoCopyNoMove.h"

class Scene;
namespace DX { class StepTimer; }
class SceneManager : private NoCopyNoMove
{
public:
	~SceneManager();
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