#include "pch.h"
#include "InputService.h"
#include "IKeyboardInputProvider.h"
#include "IMouseInputProvider.h"

InputService::~InputService() = default;
InputService::InputService(
	unique_ptr<IKeyboardInputProvider> keyboardProvider,
	unique_ptr<IMouseInputProvider> mouseProvider) noexcept :
	m_keyboardProvider{ move(keyboardProvider) },
	m_mouseProvider{ move(mouseProvider) }
{}

unique_ptr<InputService> InputService::Create(
	unique_ptr<IKeyboardInputProvider> keyboardProvider, 
	unique_ptr<IMouseInputProvider> mouseProvider)
{
	if (!keyboardProvider || !mouseProvider) return nullptr;

	unique_ptr<InputService> service(new InputService(move(keyboardProvider), move(mouseProvider)));
	return service;
}

bool InputService::IsKeyPressed(KeyCode key) const noexcept
{
	return m_keyboardState[key].IsPressed();
}

bool InputService::IsKeyHeld(KeyCode key) const noexcept
{
	return m_keyboardState[key].IsHeld();
}

bool InputService::IsKeyReleased(KeyCode key) const noexcept
{
	return m_keyboardState[key].IsReleased();
}

bool InputService::IsKeyUp(KeyCode key) const noexcept
{
	return m_keyboardState[key].IsUp();
}

bool InputService::IsMouseButtonPressed(MouseButton button) const noexcept
{
	return m_mouseState[button].IsPressed();
}

bool InputService::IsMouseButtonHeld(MouseButton button) const noexcept
{
	return m_mouseState[button].IsHeld();
}

bool InputService::IsMouseButtonReleased(MouseButton button) const noexcept
{
	return m_mouseState[button].IsReleased();
}

bool InputService::IsMouseButtonUp(MouseButton button) const noexcept
{
	return m_mouseState[button].IsUp();
}

bool InputService::AreKeysCombo(initializer_list<KeyCode> heldKeys, KeyCode pressedKey) const noexcept
{
	if (heldKeys.size() == 0) return false;

	for (KeyCode key : heldKeys)
	{
		if (!IsKeyHeld(key))
			return false;
	}

	return IsKeyPressed(pressedKey);
}

bool InputService::AreKeysMouseCombo(initializer_list<KeyCode> heldKeys, MouseButton pressedButton) const noexcept
{
	if (heldKeys.size() == 0) return false;

	for (KeyCode key : heldKeys)
	{
		if (!IsKeyHeld(key))
			return false;
	}

	return IsMouseButtonPressed(pressedButton);
}

void InputService::Update() noexcept
{
	m_keyboardProvider->Update();
	m_mouseProvider->Update();

	m_keyboardState = m_keyboardProvider->GetState();
	m_mouseState = m_mouseProvider->GetState();
}

void InputService::SetMousePositionOffset(const Point& offset) noexcept
{
	m_mouseOffset = offset;
}

KeyboardState InputService::GetKeyboardState() const noexcept 
{ 
	return m_keyboardState; 
}

MouseState InputService::GetMouseState() const noexcept 
{ 
	MouseState state = m_mouseState;

	state.position.x += m_mouseOffset.x;
	state.position.y += m_mouseOffset.y;
	state.prevPosition.x += m_mouseOffset.x;
	state.prevPosition.y += m_mouseOffset.y;

	return state;
}