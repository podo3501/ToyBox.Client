#include "pch.h"
#include "AppLoop.h"
#include "Core/Public/IRenderer.h"
#include "Utils/Profiler.h"
#include "EnvironmentLocator.h"
#include "../SerializerIO/Storage/JsonStorageLocator.h"
#include "Window/Window.h"
#include "Window/WindowProcedure.h"

AppLoop::~AppLoop()
{
    TracyShutdownProfiler();
}

AppLoop::AppLoop(unique_ptr<Window> window, unique_ptr<IRenderer> renderer, const wstring& resPath, const Vector2& resolution) :
    m_window{ move(window) },
    m_renderer{ move(renderer) }
{
    TracyStartupProfiler();

    m_environment = InitializeEnvironment(resPath, resolution);
    m_jsonStorage = InitializeJsonStorage(StorageType::File);
}

bool AppLoop::Initialize()
{
    ReturnIfFalse(InitializeClass());
    ReturnIfFalse(InitializeDerived());
    AddWinProcListener();

    ReturnIfFalse(DoPrepare());

    return true;
}

bool AppLoop::InitializeClass()
{
    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    return true;
}

void AppLoop::AddWinProcListener() noexcept
{
    m_window->AddWndProcListener([mainLoop = this](HWND wnd, UINT msg, WPARAM wp, LPARAM lp)->LRESULT {
        return mainLoop->WndProc(wnd, msg, wp, lp); });
    m_window->AddWndProcListener([](HWND wnd, UINT msg, WPARAM wp, LPARAM lp)->LRESULT {
        return MouseProc(wnd, msg, wp, lp); });
}

int AppLoop::Run()
{
    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
            Tick();

        FrameMark; //Tracy측정할때 한 프레임이 끝났다는 것을 마크하는 표시
    }

    return static_cast<int>(msg.wParam);
}

void AppLoop::Tick()
{
    m_timer.Tick([&, this]()
        {
            Update(m_timer);
        });
   
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;

    m_renderer->Draw(); //Scene(Component의 집합)을 렌더링
}

void AppLoop::OnResuming()
{
    m_timer.ResetElapsedTime();

    m_renderer->OnResuming();
}

IRenderer* AppLoop::GetRenderer() const noexcept { return m_renderer.get(); }
HWND AppLoop::GetWindowHandle() const noexcept { return m_window->GetHandle(); }
