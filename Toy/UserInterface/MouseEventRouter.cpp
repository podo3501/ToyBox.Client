#include "pch.h"
#include "MouseEventRouter.h"
#include "Locator/InputLocator.h"
#include "UserInterface/Input/IMouseEventReceiver.h"
#include "UIComponent/Traverser/UITraverser.h"

using namespace UITraverser;
void MouseEventRouter::UpdateMouseState() noexcept
{
	//hover는 공통으로 호출
	auto input = InputLocator::GetService();
	auto mouseState = input->GetMouseState();
	auto receivers = PickMouseReceivers(m_component, mouseState.pos);

	UpdateHoverState(receivers, mouseState.pos);
	ProcessCaptureComponent(mouseState); //캡쳐된 컴포넌트를 마우스 입력에 따라 처리
	CaptureComponent(mouseState);	//클릭하면 캡쳐하고 press호출
	ProcessMouseWheel(input->GetMouseWheelValue());
}

void MouseEventRouter::UpdateHoverState(vector<IMouseEventReceiver*> receivers, const XMINT2& pos) noexcept
{
	vector<IMouseEventReceiver*> hoveredReceivers;
	for (auto& receiver : receivers)
	{
		hoveredReceivers.push_back(receiver);
		receiver->OnMove(pos);
		if (receiver->OnHover() == InputResult::Consumed) break;
	}

	for (auto& prevComp : m_hoveredReceivers)
	{
		if (ranges::find(receivers, prevComp) == receivers.end())
			prevComp->OnNormal(); //영역이 아닌 애들은 OnNormal 호출
	}

	m_hoveredReceivers = hoveredReceivers;
}

void MouseEventRouter::ProcessCaptureComponent(const MouseState& mouseState) noexcept
{
	if (!m_capture) return;

	bool inside = (ranges::find(m_hoveredReceivers, m_capture) != m_hoveredReceivers.end()); //hovered에서 찾으면 inside.
	if (mouseState.leftButton == InputState::Released) //3. 마우스를 떼면 release호출하고 캡쳐해제
	{
		m_capture->OnRelease(inside);
		m_capture = nullptr;
	}

	if (mouseState.leftButton == InputState::Held)
		m_capture->OnHold(mouseState.pos, inside); //2. 캡쳐한걸 hold로 호출한다.
}

static inline bool IsHandled(InputResult result) noexcept
{
	return result == InputResult::Consumed || result == InputResult::Propagate;
}

void MouseEventRouter::CaptureComponent(const MouseState& mouseState) noexcept
{
	if (m_capture) return;
	if (mouseState.leftButton != InputState::Pressed) return;

	for (auto& receiver : m_hoveredReceivers)
	{
		const auto result = receiver->OnPress(mouseState.pos);
		if (!IsHandled(result)) continue; //처리되지 않았다면 Render상 밑의 컴포넌트

		if (result == InputResult::Consumed) //이 컴포넌트가 소비하고 다음 컴포넌트로 가지 않는다.
		{
			m_capture = receiver;
			break;
		}

		if (!m_capture && result == InputResult::Propagate)
			m_capture = receiver; //캡쳐된게 없으면 일단 얘를 캡쳐
	}
}

void MouseEventRouter::ProcessMouseWheel(int wheelValue) noexcept
{
	if (!wheelValue) return;

	for (const auto& receiver : m_hoveredReceivers)
		if (receiver->OnWheel(wheelValue))
			return; //위의 컴포넌트가 반응하면 그 밑에 컴포넌트들은 실행하지 않는다.
}