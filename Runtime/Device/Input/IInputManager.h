#pragma once
#include <memory>
#include "DirectXMath.h"
#include "DirectXTK12/Keyboard.h"

struct HWND__; 
typedef HWND__* HWND;

enum class MouseButtonState
{
    Left,
    Right,
    Middle
};

enum class InputKeyState
{
    Up,
    Held,
    Released,
    Pressed,
};

struct MouseDataState
{
    DirectX::XMINT2 pos{};
    InputKeyState leftButton{ InputKeyState::Up };
};

struct IInputManager
{
    virtual ~IInputManager() = default;
    virtual void SetMouseStartOffset(const DirectX::XMINT2& offset) noexcept = 0;
    virtual void Update() noexcept = 0;
    virtual MouseDataState GetMouseState() const noexcept = 0;
    virtual const DirectX::XMINT2& GetPosition() const noexcept = 0;

    virtual bool IsInputAction(DirectX::Keyboard::Keys key, InputKeyState inputState) noexcept = 0;
    virtual bool IsInputAction(DirectX::Keyboard::Keys key, MouseButtonState mouseButton) noexcept = 0;
    virtual bool IsInputAction(DirectX::Keyboard::Keys firstKey, DirectX::Keyboard::Keys secondKey) noexcept = 0;
    virtual bool IsInputAction(MouseButtonState mouseButton, InputKeyState keyState) noexcept = 0;
    virtual void ResetMouseWheelValue() noexcept = 0;
    virtual int GetMouseWheelValue() noexcept = 0;
};

struct IToolInputManager : public IInputManager {};

std::unique_ptr<IInputManager> CreateInputManager(HWND hwnd);
std::unique_ptr<IInputManager> CreateNullInputManager();
std::unique_ptr<IToolInputManager> CreateToolInputManager(HWND hwnd);