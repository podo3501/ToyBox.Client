#pragma once
#include "Core/Utils/Handle.h"

struct SoundTag {};
using SoundHandle = Handle<SoundTag>;
inline constexpr SoundHandle InvalidSoundHandle = InvalidHandle<SoundTag>;