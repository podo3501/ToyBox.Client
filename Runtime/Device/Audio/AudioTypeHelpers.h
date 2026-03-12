#pragma once
#include "AudioTypes.h"
#include "Core/Utils/EnumHelpers.h"
#include <array>

template<>
constexpr size_t EnumSize<SoundType>() { return 3; }

template<>
constexpr auto EnumToStringMap<SoundType>() -> std::array<const char*, EnumSize<SoundType>()> {
	return { {
		{ "Static" },
		{ "Stream" },
		{ "None" }
	} };
}

template<>
constexpr size_t EnumSize<AudioGroupID>() { return 5; }

template<>
constexpr auto EnumToStringMap<AudioGroupID>() -> std::array<const char*, EnumSize<AudioGroupID>()> {
	return { {
		{ "BGM" },
		{ "SFX" },
		{ "UI" },
		{ "System" },
		{ "None" }
	} };
}