#pragma once
#include "GameClient/Service/Input/IMouseInputProvider.h"

class DXMouseInputProvider : public IMouseInputProvider
{
public:
	~DXMouseInputProvider();
	explicit DXMouseInputProvider(DirectX::Mouse& mouse) noexcept;
	virtual void Update() noexcept override;
	virtual const MouseState& GetState() const noexcept override;

private:
	enum class UpdateMode { Sync, Normal };
	void UpdateButton(const DirectX::Mouse::State& dxState, UpdateMode mode);

	DirectX::Mouse& m_mouse;
	bool m_initialized{ false };
	MouseState m_state{};
};