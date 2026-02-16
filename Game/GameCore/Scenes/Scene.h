#pragma once
#include <string>
#include "Platform/Framework/StepTimer.h"
#include "Core/Foundation/NoCopyNoMove.h"
#include "SceneType.h"

struct IRenderer;
class UIModule;
class Scene : private NoCopyNoMove
{
public:
	virtual ~Scene() = default;
	static SceneID GetTypeStatic() { return SceneID::Unknown; }
	virtual SceneID GetTypeID() const noexcept = 0;

	virtual bool Enter() = 0;
	virtual bool Leave() = 0;
	virtual void Update(const DX::StepTimer& timer) = 0;
};
