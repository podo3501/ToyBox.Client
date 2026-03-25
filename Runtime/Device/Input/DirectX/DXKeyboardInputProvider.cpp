#include "pch.h"
#include "DXKeyboardInputProvider.h"
#include "DXKeyMap.h"

bool DXKeyboardInputProvider::Initialize() noexcept
{
    BuildKeyMap();
    return true;
}

void DXKeyboardInputProvider::BuildKeyMap() noexcept
{
    for (auto [dx, code] : DXKeyMap)
        m_DxToKeyCode[(uint32_t)dx] = code;
}

bool DXKeyboardInputProvider::IsKeyPressed(KeyCode key) const noexcept
{
    auto idx = static_cast<size_t>(key);
    if (idx >= m_inputStates.size()) return false;

    return m_inputStates[idx].IsPressed();
}

bool DXKeyboardInputProvider::IsKeyHeld(KeyCode key) const noexcept
{
    auto idx = static_cast<size_t>(key);
    if (idx >= m_inputStates.size()) return false;

    return m_inputStates[idx].IsHeld();
}

bool DXKeyboardInputProvider::IsKeyReleased(KeyCode key) const noexcept
{
    auto idx = static_cast<size_t>(key);
    if (idx >= m_inputStates.size()) return false;

    return m_inputStates[idx].IsReleased();
}

bool DXKeyboardInputProvider::IsKeyUp(KeyCode key) const noexcept
{
    auto idx = static_cast<size_t>(key);
    if (idx >= m_inputStates.size()) return false;

    return m_inputStates[idx].IsUp();
}

void DXKeyboardInputProvider::Update() noexcept
{
    auto dxState = m_keyboard.GetState();
    
    for (auto [dxKey, code] : DXKeyMap)
    {
        InputTransitionState& state = m_inputStates[static_cast<size_t>(code)];
        state.Update(dxState.IsKeyDown(dxKey));
    }
}

//////////////////////////////////////////////////

unique_ptr<IKeyboardInputProvider> CreateDXKeyboardInputProvider()
{
	return make_unique<DXKeyboardInputProvider>();
}