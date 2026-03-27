#pragma once
#include "GameClient/Service/Input/IKeyboardInputProvider.h"

class DXKeyboardInputProvider : public IKeyboardInputProvider
{
public:
	~DXKeyboardInputProvider();
	DXKeyboardInputProvider(DirectX::Keyboard& keyboard) noexcept;
	virtual void Update() noexcept override;
	virtual const KeyboardState& GetState() const noexcept override;

private:
	DirectX::Keyboard& m_keyboard;
	KeyboardState m_keyboardState{};
};