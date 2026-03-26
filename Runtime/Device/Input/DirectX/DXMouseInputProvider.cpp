#include "pch.h"
#include "DXMouseInputProvider.h"

namespace
{
    static constexpr bool DirectX::Mouse::State::* ButtonMap[] =
    {
        &DirectX::Mouse::State::leftButton,
        &DirectX::Mouse::State::rightButton,
        &DirectX::Mouse::State::middleButton,
        &DirectX::Mouse::State::xButton1,
        &DirectX::Mouse::State::xButton2
    };
}

void DXMouseInputProvider::UpdateButton(const DirectX::Mouse::State& dxState)
{
    for (int i = 0; i < (int)MouseButton::Count; ++i)
    {
        bool value = dxState.*ButtonMap[i];
        m_state.buttons[i].Update(value);
    }
}

void DXMouseInputProvider::SyncButton(const DirectX::Mouse::State& dxState)
{
    for (int i = 0; i < (int)MouseButton::Count; ++i)
    {
        bool value = dxState.*ButtonMap[i];

        m_state.buttons[i].current = value;
        m_state.buttons[i].previous = value;
    }
}

void DXMouseInputProvider::Update() noexcept
{
    auto raw = m_mouse.GetState();

    if (!m_initialized) //처음에 마우스 튀는거 방지.
    {
        SyncButton(raw);

        m_state.x = raw.x;
        m_state.y = raw.y;
        m_state.dx = 0;
        m_state.dy = 0;

        m_prevWheel = raw.scrollWheelValue;
        m_state.wheelDelta = 0;

        m_initialized = true;
        return;
    }

    UpdateButton(raw);

    int prevX = m_state.x;
    int prevY = m_state.y;

    m_state.x = raw.x;
    m_state.y = raw.y;

    m_state.dx = m_state.x - prevX;
    m_state.dy = m_state.y - prevY;

    int currentWheel = raw.scrollWheelValue;
    m_state.wheelDelta = currentWheel - m_prevWheel;
    m_prevWheel = currentWheel;
}

const MouseState& DXMouseInputProvider::GetState() const noexcept
{
    return m_state;
}

//////////////////////////////////////////////////

std::unique_ptr<IMouseInputProvider> CreateDXMouseInputProvider()
{
	return make_unique<DXMouseInputProvider>();
}