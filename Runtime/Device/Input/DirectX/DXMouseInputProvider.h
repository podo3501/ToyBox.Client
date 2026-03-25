#pragma once
#include "GameClient/Service/Input/IMouseInputProvider.h"
#include "DirectXTK12/Mouse.h"
#include "InputTransitionState.h"

class DXMouseInputProvider : public IMouseInputProvider
{
public:
	virtual bool IsMouseButtonPressed(MouseButton button) const noexcept override;
	virtual void Update() noexcept override;

private:
	DirectX::Mouse m_mouse;
	array<InputTransitionState, int(MouseButton::Count)> m_inputStates{};
};