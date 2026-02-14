#include "pch.h"
#include "GameLoop.h"
#include "Renderer/Public/IRenderer.h"
#include "Renderer/Public/IImguiRegistry.h"
#include "Platform/Window/Window.h"
#include "Locator/InputLocator.h"
#include "Locator/UIComponentLocator.h"
#include "Locator/SceneLocator.h"
#include "Locator/EventDispatcherLocator.h"
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
    InputLocator::Provide(m_inputManager.get());
    m_uiManager = UIComponentManager::Create(m_renderer);
    UIComponentLocator::Provide(m_uiManager.get());
    m_sceneManager = SceneManager::Create();
    SceneLocator::Provide(m_sceneManager.get());
    m_eventDispatcherManager = EventDispatcherManager::Create();
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