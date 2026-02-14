#include "pch.h"
#include "MouseEventReceiver.h"

void MouseEventReceiver::OnNormal() noexcept {}
InputResult MouseEventReceiver::OnHover() noexcept { return InputResult::None; }
void MouseEventReceiver::OnMove(const DirectX::XMINT2& pos) noexcept { pos; }
InputResult MouseEventReceiver::OnPress(const DirectX::XMINT2& pos) noexcept { pos; return InputResult::None; }
void MouseEventReceiver::OnHold(const DirectX::XMINT2& pos, bool inside) noexcept { pos; inside; }
void MouseEventReceiver::OnRelease(bool inside) noexcept { inside; }
bool MouseEventReceiver::OnWheel(int wheelValue) noexcept { wheelValue; return false; }

const InteractionSounds& MouseEventReceiver::GetInteractSounds() const noexcept
{
	return m_interactionSounds;
}

void MouseEventReceiver::SetInteractionSounds(InteractState interactState, UISoundSlotID slotID) noexcept
{
	m_interactionSounds[interactState] = slotID;
}
