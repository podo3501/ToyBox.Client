#pragma once
#include "KeyCode.h"
#include "MouseButton.h"

struct IKeyboardInputProvider;
struct IMouseInputProvider;

class InputService
{
public:
	~InputService();
	InputService() = delete;
	static unique_ptr<InputService> Create(
		unique_ptr<IKeyboardInputProvider> keyboardProvider, 
		unique_ptr<IMouseInputProvider> mouseProvider);
	bool IsKeyPressed(KeyCode key) noexcept;
	bool IsKeyHeld(KeyCode key) noexcept;
	bool IsKeyReleased(KeyCode key) noexcept;
	bool IsKeyUp(KeyCode key) noexcept;

	bool IsMouseButtonPressed(MouseButton button) noexcept;

private:
	InputService(
		unique_ptr<IKeyboardInputProvider> keyboardProvider,
		unique_ptr<IMouseInputProvider> mouseProvider) noexcept;

	unique_ptr<IKeyboardInputProvider> m_keyboardProvider;
	unique_ptr<IMouseInputProvider> m_mouseProvider;
};