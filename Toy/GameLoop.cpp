#include "pch.h"
#include "GameLoop.h"
#include "IRenderer.h"
#include "IImguiRegistry.h"
#include "Shared/Window/Window.h"
#include "Shared/System/Public/IInputManager.h"
#include "Locator/SceneLocator.h"
#include "Locator/EventDispatcherLocator.h"
#include "UserInterface/UIComponentLocator.h"
#include "Scenes/Test/ComponentTestScene.h"
#include "Scenes/Test/TestScene1.h"
#include "Scenes/Test/TestScene2.h"

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

GameLoop::~GameLoop() = default;
GameLoop::GameLoop(unique_ptr<Window> window, unique_ptr<IRenderer> renderer, IImguiRegistry* imguiRegistry,
    const wstring& resourcePath, const Vector2& windowSize) :
    ::AppLoop(move(window), move(renderer), imguiRegistry, resourcePath, windowSize),
    m_renderer{ AppLoop::GetRenderer() },
    m_imguiRegistry{ AppLoop::GetImguiRegistry() }
{}

bool GameLoop::InitializeDerived()
{
    m_inputManager = CreateInputManager(GetWindowHandle());
    Locator<IInputManager>::Provide(m_inputManager.get());
    m_uiManager = make_unique<UIComponentManager>(m_renderer);
    Locator<UIComponentManager>::Provide(m_uiManager.get());
    m_sceneManager = CreateSceneManager();
    SceneLocator::Provide(m_sceneManager.get());
    m_eventDispatcherManager = CreateEventDispatcherManager();
    EventDispatcherLocator::Provide(m_eventDispatcherManager.get());

    return true;
}

bool GameLoop::DoPrepare()
{
    m_sceneManager->Transition(make_unique<ComponentTestScene>(m_renderer));
    //m_sceneManager->Transition(make_unique<TestScene1>(m_renderer));

    return true;
}

void GameLoop::Update(const DX::StepTimer& timer)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");

    m_inputManager->Update();
    m_sceneManager->Update(timer);

    PIXEndEvent();
}