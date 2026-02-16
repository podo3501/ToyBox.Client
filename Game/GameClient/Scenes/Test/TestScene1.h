#pragma once
#include "GameCore/Scenes/Scene.h"

struct IRenderer;
class UIComponent;
class UIModule;
class TestScene1 : public Scene
{
public:
	~TestScene1();
	TestScene1(IRenderer* renderer);
	static SceneID GetTypeStatic() { return SceneID::TestScene1; }
	virtual SceneID GetTypeID() const noexcept override { return GetTypeStatic(); }

	virtual bool Enter() override;
	virtual bool Leave() override;
	virtual void Update(const DX::StepTimer& timer) override;

private:
	IRenderer* m_renderer{ nullptr };
	UIModule* m_uiModule{ nullptr };
};