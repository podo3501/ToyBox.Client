#pragma once
#include "AudioTypes.h"

template<>
inline constexpr auto EnumUtil::EnumToStringMap<SoundType> = std::array{
	"Static",
	"Stream"
};
ASSERT_ENUM_COUNT(SoundType);

template<>
inline constexpr auto EnumUtil::EnumToStringMap<AudioGroupID> = std::array{
	"BGM",
	"SFX",
	"UI",
	"System"
};
ASSERT_ENUM_COUNT(AudioGroupID);