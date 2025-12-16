#pragma once

class Game;

class Window
{
    using WndProcListener = std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>;

public:
    Window();
    ~Window();
    bool Create(HINSTANCE hInstance, int nCmdShow, const RECT& rc, HWND& outHwnd);
    inline void AddWndProcListener(WndProcListener listener);
    inline DirectX::SimpleMath::Vector2 GetOutputSize() const noexcept;
    inline void OnWindowSizeChanged(const RECT& size) noexcept;
    inline HWND GetHandle() const noexcept { return m_wnd; }
    static void ExitGame() noexcept;
    
private:
    bool WindowRegisterClass(HINSTANCE hInstance, const std::wstring& className);
    bool CreateGameWindow(HINSTANCE hInstance, const RECT& rc, HWND& hwnd);
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND m_wnd{};
    RECT m_outputSize{};
    HINSTANCE m_hInstance{};
    std::wstring m_className{ L"ToyWindowClass" };
    LONG m_titleBarSize{ 0 };
    std::vector<WndProcListener> m_wndProcListeners{};
};

inline void Window::AddWndProcListener(WndProcListener listener) { m_wndProcListeners.emplace_back(std::move(listener)); }
inline DirectX::SimpleMath::Vector2 Window::GetOutputSize() const noexcept{
    return { static_cast<float>(m_outputSize.right + m_outputSize.left), 
        static_cast<float>(m_outputSize.bottom + m_outputSize.top) }; }
inline void Window::OnWindowSizeChanged(const RECT& size) noexcept { m_outputSize = size; }

unique_ptr<Window> CreateWindowInstance(HINSTANCE hInstance, int nShowCmd, const RECT& rect);
