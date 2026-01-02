#pragma once
#include "Shared/Foundation/NonCopyable.h"
#include "Shared/Foundation/NonMovable.h"

class NoCopyNoMove : private NonCopyable, private NonMovable {};
