#include "pch.h"
#include "DXKeyboardInputProvider.h"
#include "DXKeyMap.h"

DirectX::Keyboard DXKeyboardInputProvider::m_keyboard;

DXKeyboardInputProvider::~DXKeyboardInputProvider() = default;
DXKeyboardInputProvider::DXKeyboardInputProvider() noexcept = default;

void DXKeyboardInputProvider::Update() noexcept
{
    const auto& dxState = m_keyboard.GetState();

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

#ifdef _WIN32
unique_ptr<IKeyboardInputProvider> CreateDXKeyboardInputProvider()
{
	return make_unique<DXKeyboardInputProvider>();
}
#endif