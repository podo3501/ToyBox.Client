#pragma once
#include "../InnerWindow.h"

namespace DX
{
    class StepTimer;
}

struct IToolInputManager;
struct IInputManager;
struct IImguiRegistry;
class Panel;
class MouseTracker;
class RenderTexture;
class TextureResourceBinder;
class ComponentController;
class UIModule;

class UserInterfaceWindow : public InnerWindow
{
public:
    UserInterfaceWindow(IRenderer* renderer, IImguiRegistry* imguiRegistry);
    ~UserInterfaceWindow();

    virtual void Render(ImGuiIO* io) override;

    bool CreateScene(const XMUINT2& size);
    bool CreateScene(const wstring& filename);
    bool SaveScene(const wstring& filename);
    wstring GetSaveFilename() const noexcept;
    void Update(const DX::StepTimer& timer);
    void ChangeWindowSize(const ImVec2& size);

    inline bool IsOpen() const noexcept { return m_isOpen; }

private:
    bool SetupProperty(UIModule* uiModule);
    void ToggleToolMode() noexcept;
    void CheckActiveUpdate(IToolInputManager* toolInput) noexcept;
    void CheckWindowMoved(IToolInputManager* toolInput) noexcept;
    void CheckWindowResized(IToolInputManager* toolInput);
    UIModule* GetUIModule() const noexcept;
    inline ImVec2 GetPanelSize() const noexcept;

    void SetupWindowAppearing() noexcept;
    void SetupImGuiWindow();
    void RenderContent();
    void ShowStatusBar() const;
    void HandleMouseEvents();

    IRenderer* m_renderer{ nullptr };
    IImguiRegistry* m_imguiRegistry{ nullptr };
    ImGuiWindow* m_window{ nullptr };
    unique_ptr<IInputManager> m_nullInputManager;
    IInputManager* m_inputManager{ nullptr };
    unique_ptr<ComponentController> m_controller;
    unique_ptr<RenderTexture> m_mainRenderTexture;

    bool m_isOpen{ false };
    bool m_isTool{ false };
    float m_fps{ 0.f };
    ImVec2 m_windowPosition{};

    static int m_uiWindowIndex;
};
