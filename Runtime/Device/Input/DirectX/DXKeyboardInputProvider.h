#pragma once
#include "GameClient/Service/Input/IKeyboardInputProvider.h"
#include "DirectXTK12/Keyboard.h"
#include "InputTransitionState.h"

class DXKeyboardInputProvider : public IKeyboardInputProvider
{
public:
	virtual bool IsKeyPressed(KeyCode key) const noexcept override;
	virtual bool IsKeyHeld(KeyCode key) const noexcept override;
	virtual bool IsKeyReleased(KeyCode key) const noexcept override;
	virtual bool IsKeyUp(KeyCode key) const noexcept override;
	virtual void Update() noexcept override;

	bool Initialize() noexcept;

private:
	void BuildKeyMap() noexcept;

	DirectX::Keyboard m_keyboard;
	array<KeyCode, int(KeyCode::Count)> m_DxToKeyCode{KeyCode::None};
	array<InputTransitionState, int(KeyCode::Count)> m_inputStates{};
};