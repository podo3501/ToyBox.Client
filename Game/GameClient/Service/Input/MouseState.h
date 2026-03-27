#pragma once
#include <array>
#include "Core/Foundation/Point.h"
#include "MouseButton.h"
#include "InputState.h"

struct MouseState
{
    Point prevPosition{};
    Point position{};
    
    int prevWheel{ 0 };
    int wheel{ 0 };

    static constexpr float WHEEL_STEP = 120.f; // 윈도우 기본 단위

    Point GetDelta() const noexcept { return { position.x - prevPosition.x, position.y - prevPosition.y }; }
    float GetWheelDelta() const noexcept { return static_cast<float>(wheel - prevWheel) / WHEEL_STEP; }

    std::array<InputState, static_cast<size_t>(MouseButton::Count)> buttons{};

    const InputState& operator[](MouseButton btn) const noexcept { return buttons[static_cast<size_t>(btn)]; }
    InputState& operator[](MouseButton btn) noexcept { return buttons[static_cast<size_t>(btn)]; }
};