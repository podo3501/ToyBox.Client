#pragma once
#include "Core/Foundation/NonCopyable.h"
#include "Core/Foundation/NonMovable.h"

class NoCopyNoMove : private NonCopyable, private NonMovable {};
