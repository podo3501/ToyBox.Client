#pragma once
#include "Shared/Foundation/NonCopyable.h"
#include "Shared/Foundation/NonMovable.h"

class ManagerBase : private NonCopyable, private NonMovable {};