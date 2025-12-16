#pragma once
#include "Shared/System/Public/AudioTypes.h"
#include "Shared/Utils/EnumHelpers.h"
#include <array>

template<>
constexpr size_t EnumSize<AudioGroupID>() { return 6; }

template<>
constexpr auto EnumToStringMap<AudioGroupID>() -> std::array<const char*, EnumSize<AudioGroupID>()> {
	return { {
		{ "Master" },
		{ "BGM" },
		{ "SFX" },
		{ "UI" },
		{ "System" },
		{ "None" }
	} };
}