#pragma once
#include <string>
#include "Shared/Framework/StepTimer.h"
#include "Shared/Foundation/NoCopyNoMove.h"
#include "SceneType.h"

struct IRenderer;
class UIModule;
class Scene : private NoCopyNoMove
{
public:
	virtual ~Scene() = default;
	Scene(IRenderer* renderer) :
		m_renderer{ renderer }
	{}
	static SceneID GetTypeStatic() { return SceneID::Unknown; }
	virtual SceneID GetTypeID() const noexcept = 0;

	virtual bool Enter() = 0;
	virtual bool Leave() = 0;
	virtual void Update(const DX::StepTimer& timer) = 0;
	inline IRenderer* GetRenderer() const noexcept { return m_renderer; }

private:
	IRenderer* m_renderer;
};
