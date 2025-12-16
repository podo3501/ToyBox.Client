#include "pch.h"
#include "AppLoop.h"
#include "Core/Public/IRenderer.h"
#include "Window/Window.h"

LRESULT AppLoop::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATEAPP:
        if (wParam)
            m_renderer->OnActivated();
        else
            m_renderer->OnDeactivated();

        Keyboard::ProcessMessage(message, wParam, lParam);
        break;

    case WM_DISPLAYCHANGE:
        m_renderer->OnDisplayChange();
        break;

    case WM_MOVE:
        m_renderer->OnWindowMoved();
        break;

    case WM_PAINT:
        if (m_sizemove)
            Tick();
        else
        {
            PAINTSTRUCT ps;
            ignore = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!m_minimized)
            {
                m_minimized = true;
                if (!m_suspend)
                    m_renderer->OnSuspending();
                m_suspend = true;
            }
        }
        else if (m_minimized)
        {
            m_minimized = false;
            if (m_suspend)
                OnResuming();
            m_suspend = false;
        }
        else //if (!m_sizemove) //창을 변경할때 업데이트 하도록 수정
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            RECT rc;
            rc.left = rc.top = 0;
            rc.right = static_cast<LONG>(width);
            rc.bottom = static_cast<LONG>(height);
            m_window->OnWindowSizeChanged(rc);
            m_renderer->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_ENTERSIZEMOVE:
        m_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        m_sizemove = false;
        {
            RECT rc;
            GetClientRect(hWnd, &rc);

            m_window->OnWindowSizeChanged(rc);
            m_renderer->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        }
        break;

    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            if (!m_suspend)
                m_renderer->OnSuspending();
            m_suspend = true;
            return TRUE;
        case PBT_APMRESUMESUSPEND:
            if (!m_minimized)
            {
                if (m_suspend)
                    OnResuming();
                m_suspend = false;
            }
            return TRUE;
        }
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (m_fullscreen)
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                const auto& resolution = m_window->GetOutputSize();
                int width = static_cast<int>(resolution.x);
                int height = static_cast<int>(resolution.y);

                ShowWindow(hWnd, SW_SHOWNORMAL);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            else
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP);
                SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                ShowWindow(hWnd, SW_SHOWMAXIMIZED);
            }

            m_fullscreen = !m_fullscreen;
        }
        Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    }

    return 0;
}