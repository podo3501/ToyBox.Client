#pragma once
#include <unordered_map>
#include "Toy/UserInterface/UIComponent/UIType.h"

enum class InputResult
{
	None, //처리안함.
	Consumed, //완전처리
	Propagate //처리했지만 부모 후속처리 필요(컴포넌트 캡쳐는 했지만 이벤트는 전파)
};

using UISoundSlotID = uint16_t;
using InteractionSounds = std::unordered_map<InteractState, UISoundSlotID>;