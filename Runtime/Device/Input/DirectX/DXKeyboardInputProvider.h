#pragma once
#include "GameClient/Service/Input/IKeyboardInputProvider.h"
#include "DirectXTK12/Keyboard.h"

class DXKeyboardInputProvider : public IKeyboardInputProvider
{
public:
	virtual void Update() noexcept override;
	virtual const KeyboardState& GetState() const noexcept override;

private:
	DirectX::Keyboard m_keyboard;
	array<KeyCode, int(KeyCode::Count)> m_DxToKeyCode{KeyCode::None};
	KeyboardState m_keyboardState{};
};