#pragma once
#include "Core/Service/Locator.h"
#include "Device/Input/IInputManager.h"

using InputLocator = Locator<IInputManager>;
using ToolInputLocator = Locator<IToolInputManager>;