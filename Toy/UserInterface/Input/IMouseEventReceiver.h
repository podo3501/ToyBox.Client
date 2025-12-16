#pragma once
#include <DirectXMath.h>

enum class InputResult
{
	None, //처리안함.
	Consumed, //완전처리
	Propagate //처리했지만 부모 후속처리 필요(컴포넌트 캡쳐는 했지만 이벤트는 전파)
};

struct IMouseEventReceiver
{
	virtual ~IMouseEventReceiver() = default;
	virtual void OnNormal() noexcept {}
	virtual InputResult OnHover() noexcept { return InputResult::None; } //밑에 컴포넌트까지 이벤트를 넘겨주지 않는다. 예를들면 OnHover 사운드가 중복해서 날 수 있다.
	virtual void OnMove(const DirectX::XMINT2& pos) noexcept { pos; }
	virtual InputResult OnPress(const DirectX::XMINT2& pos) noexcept { pos; return InputResult::None; }
	virtual void OnHold(const DirectX::XMINT2& pos, bool inside) noexcept { pos; inside; }
	virtual void OnRelease(bool inside) noexcept { inside; }
	virtual bool OnWheel(int wheelValue) noexcept { wheelValue; return false; }
};
