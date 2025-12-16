#pragma once

struct IRenderer;
struct IToolInputManager;
class UIComponent;
class ComponentTooltip;
class TextureResourceBinder;
class EditWindow;
class UICommandHistory;

class ComponentSelector
{
public:
	~ComponentSelector();
	ComponentSelector(IRenderer* renderer, TextureResourceBinder* resBinder,
		UIComponent* mainComponent, UICommandHistory* cmdHistory);

	void SetPanel(UIComponent* panel) noexcept;
	void Update() noexcept;
	void Render();
	void SetUIWindow(ImGuiWindow* uiWindow) noexcept { m_uiWindow = uiWindow; }
	UIComponent* GetComponent() { return m_component; }
	void SetComponent(UIComponent* component) noexcept;

private:
	void RepeatedSelection(const vector<UIComponent*>& componentList) noexcept;
	void SelectComponent(IToolInputManager* input) noexcept;
	bool HandleEscapeKey(IToolInputManager* input) noexcept;
	bool UpdateEditWindow() noexcept;

	IRenderer* m_renderer;
	TextureResourceBinder* m_resBinder;
	UICommandHistory* m_cmdHistory;
	ImGuiWindow* m_uiWindow;
	unique_ptr<ComponentTooltip> m_tooltip;
	unique_ptr<EditWindow> m_editWindow;

	UIComponent* m_mainComponent;
	UIComponent* m_component;
};

/////////////////////////////////////////////////////////////////

class FloatingComponent;
bool AttachSelectedComponent(UICommandHistory* cmdHistory,
	ComponentSelector* selector, FloatingComponent* floater, const XMINT2& position) noexcept;
unique_ptr<UIComponent> DetachSelectedComponent(UICommandHistory* cmdHistory, ComponentSelector* selector) noexcept;