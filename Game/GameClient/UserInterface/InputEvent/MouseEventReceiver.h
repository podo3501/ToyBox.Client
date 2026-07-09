#pragma once
#include "MouseEventType.h"

class MouseEventReceiver
{
public:
	virtual ~MouseEventReceiver() = default;
	virtual void OnNormal() noexcept;
	virtual InputResult OnHover() noexcept; //밑에 컴포넌트까지 이벤트를 넘겨주지 않는다. 예를들면 OnHover 사운드가 중복해서 날 수 있다.
	virtual void OnMove(const DirectX::XMINT2& pos) noexcept;
	virtual InputResult OnPress(const DirectX::XMINT2& pos) noexcept;
	virtual void OnHold(const DirectX::XMINT2& pos, bool inside) noexcept;
	virtual void OnRelease(bool inside) noexcept;
	virtual bool OnWheel(int wheelValue) noexcept;
	virtual const InteractionSounds& GetInteractSounds() const noexcept;

protected:
	void SetInteractionSounds(InteractState interactState, UISoundSlotID slotID) noexcept;

private:
	InteractionSounds m_interactionSounds;
};