#pragma once
#include "IRenderer.h"
#include "Shared/Framework/AppLoop.h"

struct IRenderer;
struct IToolInputManager;
struct IInputManager;
struct IEventDispatcherManager;
class Window;
class UIComponentManager;
class UserInterfaceWindow;
class TextureResBinderWindow;
class MenuBar;
namespace DX { class StepTimer; }
namespace Tool
{
	class Dialog;
	class Config;
}
class ToolLoop final : public AppLoop, public IImguiComponent
{
public:
	ToolLoop() = delete;
	ToolLoop(unique_ptr<Window> window, unique_ptr<IRenderer> renderer, IImguiRegistry* imguiRegistry, 
		const wstring& resourcePath, const Vector2& windowSize);
	virtual ~ToolLoop();
	virtual void Render(ImGuiIO* io) override;

	UserInterfaceWindow* GetFocusUIWindow() const noexcept;
	TextureResBinderWindow* GetFocusTextureResBinderWindow() const noexcept;
	void SetUIWindow(unique_ptr<UserInterfaceWindow> uiWindow) noexcept;
	void SetTextureWindow(unique_ptr<TextureResBinderWindow> textureWindow) noexcept;
	IRenderer* GetRenderer() const noexcept { return m_renderer; }
	IImguiRegistry* GetImguiRegistry() const noexcept { return m_imguiRegistry; }

protected:
	virtual bool InitializeDerived() override;
	virtual void Update(const DX::StepTimer& timer) override;

private:
	IRenderer* m_renderer{ nullptr };
	IImguiRegistry* m_imguiRegistry{ nullptr };
	unique_ptr<Tool::Config> m_config;
	unique_ptr<MenuBar> m_menuBar;
	ImTextureID m_textureID{};

	unique_ptr<IToolInputManager> m_toolInputManager;
	unique_ptr<IInputManager> m_inputManager;
	unique_ptr<UIComponentManager> m_uiManager;
	unique_ptr<IEventDispatcherManager> m_nullEventDispatcher;
	vector<unique_ptr<UserInterfaceWindow>> m_uiWindows;
	vector<unique_ptr<TextureResBinderWindow>> m_textureWindows;
};

