#pragma once
#include <memory>
#include "KeyboardState.h"

struct IKeyboardInputProvider
{
	virtual ~IKeyboardInputProvider() = default;
	virtual void Update() noexcept = 0;
	virtual const KeyboardState& GetState() const noexcept = 0;
};

#ifdef _WIN32
std::unique_ptr<IKeyboardInputProvider> CreateDXKeyboardInputProvider();
#endif