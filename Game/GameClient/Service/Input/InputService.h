#pragma once
#include "KeyboardState.h"
#include "MouseState.h"

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

	bool IsKeyPressed(KeyCode key) const noexcept;
	bool IsKeyHeld(KeyCode key) const noexcept;
	bool IsKeyReleased(KeyCode key) const noexcept;
	bool IsKeyUp(KeyCode key) const noexcept;

	bool IsMouseButtonPressed(MouseButton button) const noexcept;
	bool IsMouseButtonHeld(MouseButton button) const noexcept;
	bool IsMouseButtonReleased(MouseButton button) const noexcept;
	bool IsMouseButtonUp(MouseButton button) const noexcept;

	void Update() noexcept;

private:
	InputService(
		unique_ptr<IKeyboardInputProvider> keyboardProvider,
		unique_ptr<IMouseInputProvider> mouseProvider) noexcept;

	unique_ptr<IKeyboardInputProvider> m_keyboardProvider;
	unique_ptr<IMouseInputProvider> m_mouseProvider;

	KeyboardState m_keyboardState{};
	MouseState m_mouseState{};
};