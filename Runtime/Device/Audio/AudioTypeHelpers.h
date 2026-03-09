#pragma once
#include "AudioTypes.h"
#include "Core/Utils/EnumHelpers.h"
#include <array>

template<>
constexpr size_t EnumSize<SoundLoadMode>() { return 3; }

template<>
constexpr auto EnumToStringMap<SoundLoadMode>() -> std::array<const char*, EnumSize<SoundLoadMode>()> {
	return { {
		{ "Preload" },
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