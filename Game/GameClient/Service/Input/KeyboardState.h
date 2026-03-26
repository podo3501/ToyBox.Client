#pragma once
#include <array>
#include "KeyCode.h"
#include "InputState.h"

struct KeyboardState
{
    std::array<InputState, static_cast<size_t>(KeyCode::Count)> keys{};

    const InputState& operator[](KeyCode key) const noexcept { return keys[static_cast<size_t>(key)]; }
    InputState& operator[](KeyCode key) noexcept { return keys[static_cast<size_t>(key)]; }
};