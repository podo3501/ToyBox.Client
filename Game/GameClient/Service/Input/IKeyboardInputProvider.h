#pragma once
#include <memory>
#include "KeyCode.h"

struct IKeyboardInputProvider
{
	virtual ~IKeyboardInputProvider() = default;
	virtual bool IsKeyPressed(KeyCode key) const noexcept = 0;
	virtual bool IsKeyHeld(KeyCode key) const noexcept = 0;
	virtual bool IsKeyReleased(KeyCode key) const noexcept = 0;
	virtual bool IsKeyUp(KeyCode key) const noexcept = 0;
	virtual void Update() noexcept = 0;
};

std::unique_ptr<IKeyboardInputProvider> CreateDXKeyboardInputProvider();