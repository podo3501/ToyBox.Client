#pragma once
#include <array>
#include "MouseButton.h"
#include "InputState.h"

struct MouseState
{
    int x{ 0 };
    int y{ 0 };
    int dx{ 0 };
    int dy{ 0 };
    int wheelDelta{ 0 };

    std::array<InputState, static_cast<size_t>(MouseButton::Count)> buttons{};

    const InputState& operator[](MouseButton btn) const noexcept { return buttons[static_cast<size_t>(btn)]; }
    InputState& operator[](MouseButton btn) noexcept { return buttons[static_cast<size_t>(btn)]; }
};