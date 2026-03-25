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

bool InputService::IsKeyPressed(KeyCode key) noexcept
{
	return m_keyboardProvider->IsKeyPressed(key);
}

bool InputService::IsKeyHeld(KeyCode key) noexcept
{
	return m_keyboardProvider->IsKeyHeld(key);
}

bool InputService::IsKeyReleased(KeyCode key) noexcept
{
	return m_keyboardProvider->IsKeyReleased(key);
}

bool InputService::IsKeyUp(KeyCode key) noexcept
{
	return m_keyboardProvider->IsKeyUp(key);
}

bool InputService::IsMouseButtonPressed(MouseButton button) noexcept
{
	return m_mouseProvider->IsMouseButtonPressed(button);
}
