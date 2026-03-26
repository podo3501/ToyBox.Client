#pragma once
#include "../IInputManager.h"
#include "DirectXTK12/Keyboard.h"
#include "DirectXTK12/Mouse.h"

//IInputManager는 게임내에서 사용, IToolInputManager는 툴에서 사용한다. 
//IInputManager를 상속받았기 때문에 IInputManager도 사용할 수있다.
class InputManager : public IToolInputManager, private NoCopyNoMove
{
public:
    virtual ~InputManager();
    InputManager();
    void Initialize(HWND hwnd);

    virtual void SetMouseStartOffset(const XMINT2& offset) noexcept override;
    virtual void Update() noexcept override;
    virtual MouseDataState GetMouseState() const noexcept override;
    virtual const XMINT2& GetPosition() const noexcept override;

    virtual bool IsInputAction(Keyboard::Keys key, InputKeyState inputState) noexcept override;
    virtual bool IsInputAction(Keyboard::Keys key, MouseButtonState mouseButton) noexcept override;
    virtual bool IsInputAction(Keyboard::Keys firstKey, Keyboard::Keys secondKey) noexcept override;
    virtual bool IsInputAction(MouseButtonState mouseButton, InputKeyState keyState) noexcept override;
    virtual void ResetMouseWheelValue() noexcept override;
    virtual int GetMouseWheelValue() noexcept override;

private:
    void SetPosition(XMINT2 position) noexcept { m_position = position; }

    Keyboard::KeyboardStateTracker m_keyboardTracker{};
    Mouse::ButtonStateTracker m_mouseTracker{};

    XMINT2 m_startOffset{};
    XMINT2 m_position{};
    int m_lastMouseWheelValue{ 0 };
};

class NullInputManager : public IInputManager, private NoCopyNoMove
{
public:
    ~NullInputManager() = default;
    NullInputManager() = default;

    virtual void SetMouseStartOffset(const DirectX::XMINT2& offset) noexcept override {};
    virtual void Update() noexcept override {}
    virtual MouseDataState GetMouseState() const noexcept override { return {}; }
    virtual const DirectX::XMINT2& GetPosition() const noexcept override
    {
        static DirectX::XMINT2 dummy{ 0, 0 };
        return dummy;
    }

    virtual bool IsInputAction(DirectX::Keyboard::Keys, InputKeyState) noexcept override { return false; }
    virtual bool IsInputAction(DirectX::Keyboard::Keys, MouseButtonState) noexcept override { return false; }
    virtual bool IsInputAction(DirectX::Keyboard::Keys, DirectX::Keyboard::Keys) noexcept override { return false; }
    virtual bool IsInputAction(MouseButtonState, InputKeyState) noexcept override { return false; }
    virtual void ResetMouseWheelValue() noexcept override {}
    virtual int GetMouseWheelValue() noexcept override { return 0; }
};