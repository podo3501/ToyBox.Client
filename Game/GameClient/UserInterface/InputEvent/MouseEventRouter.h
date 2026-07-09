#pragma once

struct MouseDataState;
class UIComponent;
class MouseEventReceiver;
class MouseEventRouter : private NoCopyNoMove
{
public:
	inline void SetComponent(UIComponent* component) noexcept { m_component = component; }
	void UpdateMouseState() noexcept;

private:
	void UpdateHoverState(vector<MouseEventReceiver*> receivers, const XMINT2& pos) noexcept;
	void ProcessCaptureComponent(const MouseDataState& mouseState) noexcept;
	void CaptureComponent(const MouseDataState& mouseState) noexcept;
	void ProcessMouseWheel(int wheelValue) noexcept;

	UIComponent* m_component{ nullptr };
	vector<MouseEventReceiver*> m_hoveredReceivers; //이전 호버된 컴포넌트와 비교해서 OnNormal 호출함.
	MouseEventReceiver* m_capture{ nullptr };
};