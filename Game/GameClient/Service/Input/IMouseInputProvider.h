#pragma once
#include <memory>
#include "MouseState.h"

struct IMouseInputProvider
{
	virtual ~IMouseInputProvider() = default;
	virtual void Update() noexcept = 0;
	virtual const MouseState& GetState() const noexcept = 0;
};

#ifdef _WIN32
namespace DirectX { class Mouse; }
std::unique_ptr<IMouseInputProvider> CreateDXMouseInputProvider(DirectX::Mouse& mouse);
#endif