#include "pch.h"
#include "ToolLoop.h"
#include "Config/Config.h"
#include "Window/UserInterface/UserInterfaceWindow.h"
#include "Window/TextureResourceBinder/TextureResBinderWindow.h"
#include "Window/Menu/MenuBar.h"
#include "Window/Dialog.h"
#include "Core/Public/IImguiRegistry.h"
#include "Shared/Window/Window.h"
#include "Toy/Locator/EventDispatcherLocator.h"
#include "Toy/Locator/InputLocator.h"
#include "Toy/UserInterface/UIComponentLocator.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(disable : 4061)

#ifdef USING_D3D12_AGILITY_SDK
extern "C"
{
    // Used to enable the "Agility SDK" components
    __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION;
    __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\";
}
#endif

ToolLoop::~ToolLoop() = default;
ToolLoop::ToolLoop(unique_ptr<Window> window, unique_ptr<IRenderer> renderer, unique_ptr<IImguiRegistry> imguiRegistry,
    const wstring& resourcePath, const Vector2& windowSize) :
    ::AppLoop(move(window), move(renderer), move(imguiRegistry), resourcePath, windowSize),
    m_renderer{ AppLoop::GetRenderer() },
    m_imguiRegistry{ AppLoop::GetImguiRegistry() }
{
    m_menuBar = make_unique<MenuBar>(this);
    m_imguiRegistry->AddComponent(this);
}

bool ToolLoop::InitializeDerived()
{
    HWND hWnd = GetWindowHandle();

    m_toolInputManager = CreateToolInputManager(hWnd);
    ToolInputLocator::Provide(m_toolInputManager.get());

    m_inputManager = CreateInputManager(hWnd);
    InputLocator::Provide(m_inputManager.get());

    m_uiManager = make_unique<UIComponentManager>(m_renderer, true);
    UIComponentLocator::Provide(m_uiManager.get());

    m_nullEventDispatcher = CreateNullEventDispatcherManager();
    EventDispatcherLocator::Provide(m_nullEventDispatcher.get());

    return true;
}

void ToolLoop::SetUIWindow(unique_ptr<UserInterfaceWindow> uiWindow) noexcept
{
    m_uiWindows.emplace_back(move(uiWindow));
}

void ToolLoop::SetTextureWindow(unique_ptr<TextureResBinderWindow> textureWindow) noexcept
{
    m_textureWindows.emplace_back(move(textureWindow));
}

template <typename InnerWindow>
static InnerWindow* GetFocusWindow(const vector<unique_ptr<InnerWindow>>& windows) noexcept
{
    auto it = ranges::max_element(
        windows,
        [](const auto& rhs, const auto& lhs) {
            const ImGuiWindow* wRhs = rhs->GetImGuiWindow();
            const ImGuiWindow* wlhs = lhs->GetImGuiWindow();
            return wRhs->FocusOrder < wlhs->FocusOrder;
        });

    if (it == windows.end())
        return nullptr;

    return it->get();
}

UserInterfaceWindow* ToolLoop::GetFocusUIWindow() const noexcept
{
    return GetFocusWindow(m_uiWindows);
}

TextureResBinderWindow* ToolLoop::GetFocusTextureResBinderWindow() const noexcept
{
    return GetFocusWindow(m_textureWindows);
}

void ToolLoop::Update(const DX::StepTimer& timer)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");
    UNREFERENCED_PARAMETER(timer);

    m_toolInputManager->Update();
    m_inputManager->Update();
    m_menuBar->Update();

    erase_if(m_uiWindows, [](auto& wnd) { return !wnd->IsOpen(); });
    ranges::for_each(m_uiWindows, [&timer](const auto& wnd) {
        wnd->Update(timer);
        });

    erase_if(m_textureWindows, [](auto& texWnd) { return !texWnd->IsOpen(); });
    ranges::for_each(m_textureWindows, [](const auto& texWnd) {
        texWnd->Update();
        });

    PIXEndEvent();
}

void ToolLoop::Render(ImGuiIO* io)
{
    Tool::Dialog::Render();
    m_menuBar->Render();

    return;
}