#pragma once
#include "Core/Utils/HandleAllocator.h"

struct VoiceTag {};
using VoiceHandle = GenerationalHandle<VoiceTag>;
inline constexpr VoiceHandle InvalidVoiceHandle = GenerationalHandle<VoiceTag>::Invalid();