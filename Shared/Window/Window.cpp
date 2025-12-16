#include "pch.h"
#include "Window.h"

LPCWSTR g_szAppName = L"Toy";

void ExitGame() noexcept;

//혹시나 있을 큐 안에 메세지 내용을 비운다.
void ClearWindowMessageQueue()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {}
}

Window::Window()
{
    AddWndProcListener([wnd = this](HWND w, UINT m, WPARAM wp, LPARAM lp)->LRESULT {
        return wnd->MsgProc(w, m, wp, lp); });
}

Window::~Window()
{
    m_wndProcListeners.clear();
    DestroyWindow(m_wnd);
    UnregisterClass(m_className.c_str(), m_hInstance);
    ClearWindowMessageQueue();
}

bool Window::Create(HINSTANCE hInstance, int nCmdShow, const RECT& rc, HWND& outHwnd)
{
    ReturnIfFalse(WindowRegisterClass(hInstance, m_className));
    ReturnIfFalse(CreateGameWindow(hInstance, rc, outHwnd));

    m_wnd = outHwnd;
    m_hInstance = hInstance;

    ShowWindow(outHwnd, nCmdShow);
    // TODO: Change nCmdShow to SW_SHOWMAXIMIZED to default to fullscreen.

    //GetClientRect(hwnd, &rc);

    return true;
}

Window* gWindow = nullptr;
bool Window::WindowRegisterClass(HINSTANCE hInstance, const std::wstring& className)
{
    gWindow = this;
    //캡쳐가 있으면( [this]하면 될것 같으나 ) WNDPROC으로 변환이 되지 않는다. 그래서 정적변수활용
    auto windowProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT {
        return gWindow->WndProc(hwnd, msg, wParam, lParam);
        };

    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = windowProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszClassName = className.c_str();
    wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
    if (!RegisterClassExW(&wcex))
        return false;

    return true;
}

bool Window::CreateGameWindow(HINSTANCE hInstance, const RECT& rc, HWND& hwnd)
{
    //rc = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };

    RECT rect{ rc };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    hwnd = CreateWindowExW(0, L"ToyWindowClass", g_szAppName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hInstance, nullptr);
    // TODO: Change to CreateWindowExW(WS_EX_TOPMOST, L"ToyWindowClass", g_szAppName, WS_POPUP,
    // to default to fullscreen.
    if (!hwnd) return false;

    m_outputSize = rc;

    return true;
}

// Windows procedure
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    for (auto wndProcListener : m_wndProcListeners)
    {
        LRESULT result{ 0 };
        result = wndProcListener(hWnd, message, wParam, lParam);
        if (result != 0)
            return result;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK Window::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);

    switch (message)
    {
    case WM_CREATE:
        if (lParam)
        {
            auto params = reinterpret_cast<LPCREATESTRUCTW>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(params->lpCreateParams));
        }
        break;

    case WM_GETMINMAXINFO:
        if (lParam)
        {
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
    }
    
    return 0;
}

// Exit helper
void Window::ExitGame() noexcept
{
    if (gWindow->m_wnd == nullptr)
        return;

    PostMessage(gWindow->m_wnd, WM_CLOSE, 0, 0);
}

unique_ptr<Window> CreateWindowInstance(HINSTANCE hInstance, int nShowCmd, const RECT& rect)
{
    auto window = make_unique<Window>();
    HWND hwnd{};
    if (!window->Create(hInstance, nShowCmd, rect, hwnd))
        return nullptr;
    return window;
}

