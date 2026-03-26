#pragma once
#include "GameClient/Service/Input/IMouseInputProvider.h"
#include "DirectXTK12/Mouse.h"

class DXMouseInputProvider : public IMouseInputProvider
{
public:
	virtual void Update() noexcept override;
	virtual const MouseState& GetState() const noexcept override;

private:
	void SyncButton(const DirectX::Mouse::State& dxState);
	void UpdateButton(const DirectX::Mouse::State& dxState);

	DirectX::Mouse m_mouse;
	array<InputState, int(MouseButton::Count)> m_inputStates{};
	bool m_initialized{ false };
	MouseState m_state{};
	int m_prevWheel{ 0 };
};