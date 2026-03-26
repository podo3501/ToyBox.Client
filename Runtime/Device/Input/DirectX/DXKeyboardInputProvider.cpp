#include "pch.h"
#include "DXKeyboardInputProvider.h"
#include "DXKeyMap.h"

void DXKeyboardInputProvider::Update() noexcept
{
    auto dxState = m_keyboard.GetState();

    for (auto [dxKey, code] : DXKeyMap)
    {
        InputState& state = m_keyboardState[code];
        state.Update(dxState.IsKeyDown(dxKey));
    }
}

const KeyboardState& DXKeyboardInputProvider::GetState() const noexcept 
{ 
    return m_keyboardState; 
}

//////////////////////////////////////////////////

unique_ptr<IKeyboardInputProvider> CreateDXKeyboardInputProvider()
{
	return make_unique<DXKeyboardInputProvider>();
}