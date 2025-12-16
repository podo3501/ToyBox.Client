#pragma once
#include <string>
#include <memory>
#include "Shared/System/StepTimer.h"

class Scene;
struct ISceneManager
{
	virtual ~ISceneManager() = default;
	virtual void Transition(std::unique_ptr<Scene> newScene) = 0;
	virtual void Update(const DX::StepTimer& timer) = 0;
};

std::unique_ptr<ISceneManager> CreateSceneManager();