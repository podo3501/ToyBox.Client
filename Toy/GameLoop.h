#pragma once
#include "Shared/Framework/AppLoop.h"

struct IInputManager;
struct ISceneManager;
struct IEventDispatcherManager;
struct IImguiRegistry;
class UIComponentManager;
class UIComponent;
class GameLoop final : public AppLoop
{
public:
	GameLoop() = delete;
	GameLoop(unique_ptr<Window> window, unique_ptr<IRenderer> renderer, unique_ptr<IImguiRegistry> imguiRegistry,
		const wstring& resourcePath, const Vector2& windowSize);
	virtual ~GameLoop();

protected:
	virtual bool InitializeDerived() override;
	virtual bool DoPrepare() override;
	virtual void Update(const DX::StepTimer& timer) override;

private:
	bool AttachComponentToPanel(unique_ptr<UIComponent> component, const XMINT2& position) const noexcept;

	IRenderer* m_renderer{ nullptr };
	IImguiRegistry* m_imguiRegistry{ nullptr };
	unique_ptr<IInputManager> m_inputManager;
	unique_ptr<UIComponentManager> m_uiManager;
	unique_ptr<ISceneManager> m_sceneManager;
	unique_ptr<IEventDispatcherManager> m_eventDispatcherManager;
};