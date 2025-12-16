#include "pch.h"
#include "InputManager.h"

//속도를 위해서 function대신에 함수 포인터를 사용함
using InputActionFunc = bool(*)(const Keyboard::KeyboardStateTracker&, Keyboard::Keys);
constexpr InputActionFunc KeyboardActions[] =
{
    [](const Keyboard::KeyboardStateTracker&, Keyboard::Keys) { return false; },   //키보드에는 UP에 대한 함수가 없음.
    [](const Keyboard::KeyboardStateTracker& keyboard, Keyboard::Keys key) { return keyboard.GetLastState().IsKeyDown(key); },
    [](const Keyboard::KeyboardStateTracker& keyboard, Keyboard::Keys key) { return keyboard.IsKeyReleased(key); },
    [](const Keyboard::KeyboardStateTracker& keyboard, Keyboard::Keys key) { return keyboard.IsKeyPressed(key); }
};

/////////////////////////////////////////////////////////////////////////////

//속도를 위해 함수 포인터로 구현함.
using MouseButtonStateChecker = bool(*)(const Mouse::ButtonStateTracker&, Mouse::ButtonStateTracker::ButtonState);
static bool CheckMouseLeftButton(const Mouse::ButtonStateTracker& tracker, Mouse::ButtonStateTracker::ButtonState state) noexcept {
    return tracker.leftButton == state;
}

static bool CheckMouseRightButton(const Mouse::ButtonStateTracker& tracker, Mouse::ButtonStateTracker::ButtonState state) noexcept {
    return tracker.rightButton == state;
}

static bool CheckMouseMiddleButton(const Mouse::ButtonStateTracker& tracker, Mouse::ButtonStateTracker::ButtonState state) noexcept {
    return tracker.middleButton == state;
}

static inline Mouse::ButtonStateTracker::ButtonState GetMouseKeyState(InputState inputState)
{
    //두 enum의 값이 동일하다.
    return static_cast<Mouse::ButtonStateTracker::ButtonState>(inputState);
}

static bool IsMouseButtonState(
    const Mouse::ButtonStateTracker& mouseTracker, 
    MouseButton mouseButton, 
    Mouse::ButtonStateTracker::ButtonState state) noexcept
{
    constexpr MouseButtonStateChecker mouseButtonCheckers[] = {
        &CheckMouseLeftButton,
        &CheckMouseRightButton,
        &CheckMouseMiddleButton
    };

    return mouseButtonCheckers[static_cast<size_t>(mouseButton)](mouseTracker, state);
}

/////////////////////////////////////////////////////////////////////////////

//키보드와 마우스는 한번만 생성되어야 하기 때문에 이렇게 생성한다.
DirectX::Keyboard g_keyboard;
DirectX::Mouse g_mouse;

InputManager::~InputManager()
{
}

InputManager::InputManager() = default;

void InputManager::Initialize(HWND hwnd)
{
    g_mouse.SetWindow(hwnd);
}

void InputManager::SetMouseStartOffset(const XMINT2& offset) noexcept
{
    m_startOffset = offset;
}

void InputManager::Update() noexcept
{
    m_keyboardTracker.Update(g_keyboard.GetState());
    DirectX::Mouse::State offset(g_mouse.GetState());
    offset.x -= m_startOffset.x;
    offset.y -= m_startOffset.y;
    SetPosition({ offset.x, offset.y });
    m_mouseTracker.Update(offset);
}

MouseState InputManager::GetMouseState() const noexcept
{
    auto dxState = g_mouse.GetState();
    return { m_position, static_cast<InputState>(m_mouseTracker.leftButton) };
}

const XMINT2& InputManager::GetPosition() const noexcept
{
    return m_position;
}

bool InputManager::IsInputAction(Keyboard::Keys key, InputState inputState) noexcept
{
    return KeyboardActions[static_cast<size_t>(inputState)](m_keyboardTracker, key);
}

bool InputManager::IsInputAction(Keyboard::Keys key, MouseButton mouseButton) noexcept
{
    //앞 키는 hold 하고 뒤에는 pressed를 해야 한다. 둘다 pressed를 하면 동시에 눌러야 하기 때문에 키가 안 먹힌다.
    return IsInputAction(key, InputState::Held) && IsInputAction(mouseButton, InputState::Pressed);
}

bool InputManager::IsInputAction(Keyboard::Keys firstKey, Keyboard::Keys secondKey) noexcept
{
    bool firstKeyHeld = IsInputAction(firstKey, InputState::Held);
    bool secondKeyPressed = IsInputAction(secondKey, InputState::Pressed);

    return firstKeyHeld && secondKeyPressed;
    //눌리고 있을때 너무 빨리 이벤트가 들어간다. 그래서 시간 지연을 줄 생각이었는데 눌렀을때와 눌리고 있
    //을때 두개의 시간간격이 없어서 한번씩 누르기가 어렵게 된다. 일단은 하나씩 하는 걸로 하고 추후에 이
    //부분을 개선해서 계속 키를 눌렀을때 간격과, 눌렀을 때 눌리고 있을때 이 사이의 간격을 지정하게끔 구현하자.
}

bool InputManager::IsInputAction(MouseButton mouseButton, InputState inputState) noexcept
{
    return IsMouseButtonState(m_mouseTracker, mouseButton, GetMouseKeyState(inputState));
}

void InputManager::ResetMouseWheelValue() noexcept
{
    m_lastMouseWheelValue = m_mouseTracker.GetLastState().scrollWheelValue;
}

int InputManager::GetMouseWheelValue() noexcept
{
    const int& curWheelValue = m_mouseTracker.GetLastState().scrollWheelValue;
    int delta = (curWheelValue - m_lastMouseWheelValue) / 120;
    m_lastMouseWheelValue = curWheelValue;

    return delta;
}

/////////////////////////////////////////////////////////////////////////////

unique_ptr<IInputManager> CreateInputManager(HWND hwnd)
{
    auto input = make_unique<InputManager>();
    input->Initialize(hwnd);
    return input;
}

unique_ptr<IInputManager> CreateNullInputManager()
{
    auto input = make_unique<NullInputManager>();
    return input;
}

unique_ptr<IToolInputManager> CreateToolInputManager(HWND hwnd)
{
    auto input = make_unique<InputManager>();
    input->Initialize(hwnd);
    return input;
}