#pragma once

struct IRenderer;
struct IToolInputManager;
class UICommandHistory;
class UIComponent;
class TextureResourceBinder;
class RenameNotifier;
class MouseTracker;
enum class OnDrag;
namespace Tool{
	class MouseCursor;
}

class EditWindow
{
public:
	EditWindow() = delete;
	EditWindow(UIComponent* component, UICommandHistory* cmdHistory) noexcept;
	virtual ~EditWindow();
	
	void Setup();
	void Update(bool mainWndFocus);
	void Render(const ImGuiWindow* uiWindow);
	inline bool IsVisible() const noexcept { return m_visible; };
	bool IsUpdateSizeOnDrag() const noexcept;

protected:
	virtual void SetupComponent() {};
	virtual void UpdateComponent() {};
	virtual void RenderComponent() {};

	inline UICommandHistory* GetUICommandHistory() const noexcept { return m_cmdHistory; }
	inline UIComponent* GetComponent() const noexcept { return m_component; }

private:
	void RenderCommon();
	void ResizeComponentOnClick() noexcept;
	void UpdateDragState(IToolInputManager* toolInput, OnDrag dragState, XMINT2& outStartPos) noexcept;
	void ResizeComponent(const XMINT2& startPos, const XMINT2& currPos) noexcept;

	UIComponent* m_component;
	UICommandHistory* m_cmdHistory;
	unique_ptr<RenameNotifier> m_renameNotifier;
	bool m_visible{ true };
	OnDrag m_dragState;
};

class Panel;
class EditPanel : public EditWindow
{
public:
	~EditPanel();
	EditPanel() = delete;
	EditPanel(Panel* panel, UICommandHistory* cmdHistory) noexcept;
	//virtual void SetComponent(UIComponent* component);
	//virtual void Render();

private:
};