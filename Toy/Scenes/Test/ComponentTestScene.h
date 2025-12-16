#pragma once
#include "../Scene.h"

class UIComponent;
class UIModule;
class ComponentTestScene : public Scene
{
public:
	ComponentTestScene(IRenderer* renderer);
	static SceneID GetTypeStatic() { return SceneID::ComponentTest; }
	virtual SceneID GetTypeID() const noexcept override { return GetTypeStatic(); }

	virtual bool Enter() override;
	virtual bool Leave() override;
	virtual void Update(const DX::StepTimer& timer) override;

private:
	bool LoadResources();
	bool AttachComponentToPanel(unique_ptr<UIComponent> component, const XMINT2& position) const noexcept;

	//unique_ptr<UIModule> m_uiModule;
	UIModule* m_uiModule{ nullptr };
};