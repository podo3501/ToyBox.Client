#pragma once
#include "Shared/Framework/Locator.h"
#include "Device/Input/IInputManager.h"

using InputLocator = Locator<IInputManager>;
using ToolInputLocator = Locator<IToolInputManager>;