#pragma once
#include "GameCore/Scenes/Scene.h"

struct IResourceManager;
class UIComponent;
class UIModule;
class TestScene2 : public Scene
{
public:
	TestScene2(IResourceManager* resManager, IRenderer* renderer);
	static SceneID GetTypeStatic() { return SceneID::TestScene2; }
	virtual SceneID GetTypeID() const noexcept override { return GetTypeStatic(); }

	virtual bool Enter() override;
	virtual bool Leave() override;
	virtual void Update(const DX::StepTimer& timer) override;

private:
	IResourceManager* m_resManager{ nullptr };
	IRenderer* m_renderer{ nullptr };
	UIModule* m_uiModule{ nullptr };
};