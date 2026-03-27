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

	bool AreKeysCombo(initializer_list<KeyCode> heldKeys, KeyCode pressedKey) const noexcept;
	bool AreKeysMouseCombo(initializer_list<KeyCode> heldKeys, MouseButton pressedButton) const noexcept;

	void Update() noexcept;

	void SetMousePositionOffset(const Point& offset) noexcept;
	KeyboardState GetKeyboardState() const noexcept;
	MouseState GetMouseState() const noexcept;

private:
	InputService(
		unique_ptr<IKeyboardInputProvider> keyboardProvider,
		unique_ptr<IMouseInputProvider> mouseProvider) noexcept;

	unique_ptr<IKeyboardInputProvider> m_keyboardProvider;
	unique_ptr<IMouseInputProvider> m_mouseProvider;

	KeyboardState m_keyboardState{};
	MouseState m_mouseState{};

	Point m_mouseOffset{};
};