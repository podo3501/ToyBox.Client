#pragma once
#include <cstdint>

enum class KeyCode : uint8_t
{
    None = 0,

    // --- Keyboard ---
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    
    // --- Basic ---
    Space,
    Enter,
    Escape,
    Tab,
    Backspace,

    // --- Modifiers ---
    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,

    // --- Arrow ---
    Up,
    Down,
    Left,
    Right,

    // --- QWER 위에 붙어 있는 숫자. NumPad0과 다름. ---
    Digit0,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    Digit5,
    Digit6,
    Digit7,
    Digit8,
    Digit9,

    // --- Function Keys ---
    F1, F2, F3, F4, F5, F6,
    F7, F8, F9, F10, F11, F12,

    Count
};

enum class KeyState
{
    Pressed,
    Held,
    Released,
    Up,
};