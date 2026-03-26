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

void InputService::Update() noexcept
{
	m_keyboardProvider->Update();
	m_mouseProvider->Update();

	m_keyboardState = m_keyboardProvider->GetState();
	m_mouseState = m_mouseProvider->GetState();
}
