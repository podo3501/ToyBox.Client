#pragma once
#include "Core/Utils/Handle.h"

struct InstanceTag {};
using InstanceHandle = Handle<InstanceTag>;
inline constexpr InstanceHandle InvalidInstanceHandle = InvalidHandle<InstanceTag>;