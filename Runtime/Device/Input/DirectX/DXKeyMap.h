#pragma once
#include <utility>
#include "DirectXTK12/Keyboard.h"
#include "GameClient/Service/Input/KeyCode.h"

using DXKey = DirectX::Keyboard::Keys;

constexpr std::pair<DXKey, KeyCode> DXKeyMap[] = {
    { DXKey::A, KeyCode::A },
    { DXKey::B, KeyCode::B },
    { DXKey::C, KeyCode::C },
    { DXKey::D, KeyCode::D },
    { DXKey::E, KeyCode::E },
    { DXKey::F, KeyCode::F },
    { DXKey::G, KeyCode::G },
    { DXKey::H, KeyCode::H },
    { DXKey::I, KeyCode::I },
    { DXKey::J, KeyCode::J },
    { DXKey::K, KeyCode::K },
    { DXKey::L, KeyCode::L },
    { DXKey::M, KeyCode::M },
    { DXKey::N, KeyCode::N },
    { DXKey::O, KeyCode::O },
    { DXKey::P, KeyCode::P },
    { DXKey::Q, KeyCode::Q },
    { DXKey::R, KeyCode::R },
    { DXKey::S, KeyCode::S },
    { DXKey::T, KeyCode::T },
    { DXKey::U, KeyCode::U },
    { DXKey::V, KeyCode::V },
    { DXKey::W, KeyCode::W },
    { DXKey::X, KeyCode::X },
    { DXKey::Y, KeyCode::Y },
    { DXKey::Z, KeyCode::Z },

    { DXKey::D0, KeyCode::Digit0 },
    { DXKey::D1, KeyCode::Digit1 },
    { DXKey::D2, KeyCode::Digit2 },
    { DXKey::D3, KeyCode::Digit3 },
    { DXKey::D4, KeyCode::Digit4 },
    { DXKey::D5, KeyCode::Digit5 },
    { DXKey::D6, KeyCode::Digit6 },
    { DXKey::D7, KeyCode::Digit7 },
    { DXKey::D8, KeyCode::Digit8 },
    { DXKey::D9, KeyCode::Digit9 },

    { DXKey::Space, KeyCode::Space },
    { DXKey::Escape, KeyCode::Escape },
    { DXKey::Enter, KeyCode::Enter },
    { DXKey::LeftShift, KeyCode::LeftShift },
    { DXKey::RightShift, KeyCode::RightShift },
    // 필요한 키 추가
};
