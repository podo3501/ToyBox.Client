#pragma once
#include <memory>
#include "MouseButton.h"

struct IMouseInputProvider
{
	virtual ~IMouseInputProvider() = default;
	virtual bool IsMouseButtonPressed(MouseButton button) const noexcept = 0;
	virtual void Update() noexcept = 0;
};

std::unique_ptr<IMouseInputProvider> CreateDXMouseInputProvider();