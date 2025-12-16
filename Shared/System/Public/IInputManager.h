#pragma once
#include <memory>
#include "DirectXMath.h"
#include "DirectXTK12/Keyboard.h"

struct HWND__; 
typedef HWND__* HWND;

enum class MouseButton
{
    Left,
    Right,
    Middle
};

enum class InputState
{
    Up,
    Held,
    Released,
    Pressed,
};

struct MouseState
{
    DirectX::XMINT2 pos{};
    InputState leftButton{ InputState::Up };
};

struct IInputManager
{
    virtual ~IInputManager() = default;
    virtual void SetMouseStartOffset(const DirectX::XMINT2& offset) noexcept = 0;
    virtual void Update() noexcept = 0;
    virtual MouseState GetMouseState() const noexcept = 0;
    virtual const DirectX::XMINT2& GetPosition() const noexcept = 0;

    virtual bool IsInputAction(DirectX::Keyboard::Keys key, InputState inputState) noexcept = 0;
    virtual bool IsInputAction(DirectX::Keyboard::Keys key, MouseButton mouseButton) noexcept = 0;
    virtual bool IsInputAction(DirectX::Keyboard::Keys firstKey, DirectX::Keyboard::Keys secondKey) noexcept = 0;
    virtual bool IsInputAction(MouseButton mouseButton, InputState keyState) noexcept = 0;
    virtual void ResetMouseWheelValue() noexcept = 0;
    virtual int GetMouseWheelValue() noexcept = 0;
};

struct IToolInputManager : public IInputManager {};

std::unique_ptr<IInputManager> CreateInputManager(HWND hwnd);
std::unique_ptr<IInputManager> CreateNullInputManager();
std::unique_ptr<IToolInputManager> CreateToolInputManager(HWND hwnd);