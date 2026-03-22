#pragma once
#include "Core/Utils/IDHandle.h"

struct SoundTag {};
using SoundHandle = IDHandle<SoundTag>;
inline constexpr SoundHandle InvalidSoundHandle = IDHandle<SoundTag>::Invalid();