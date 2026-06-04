#include "pch.h"
#include "DXMouseInputProvider.h"
#include "DirectXTK12/Mouse.h"

DirectX::Mouse DXMouseInputProvider::m_mouse;

inline constexpr bool DirectX::Mouse::State::* ButtonMap[] =
{
    &DirectX::Mouse::State::leftButton,
    &DirectX::Mouse::State::rightButton,
    &DirectX::Mouse::State::middleButton,
    &DirectX::Mouse::State::xButton1,
    &DirectX::Mouse::State::xButton2
};
inline constexpr size_t ButtonCount = std::size(ButtonMap);
static_assert(ButtonCount == static_cast<size_t>(MouseButton::Count), "ButtonMap and MouseState::buttons size mismatch");

DXMouseInputProvider::~DXMouseInputProvider() = default;
DXMouseInputProvider::DXMouseInputProvider(HWND hwnd) noexcept
{
    m_mouse.SetWindow(hwnd);
}

void DXMouseInputProvider::UpdateButton(const DirectX::Mouse::State& dxState, UpdateMode mode)
{
    for (size_t i = 0; i < ButtonCount; ++i)
    {
        auto value = dxState.*ButtonMap[i];

        if (mode == UpdateMode::Sync)
        {
            m_state.buttons[i].current = value;
            m_state.buttons[i].previous = value;
            continue;
        }

        m_state.buttons[i].Update(value);
    }
}

void DXMouseInputProvider::Update() noexcept
{
    const auto& raw = m_mouse.GetState();

    if (!m_initialized) //처음에 마우스 튀는거 방지.
    {
        UpdateButton(raw, UpdateMode::Sync);

        auto rawPos = ToPoint(raw.x, raw.y);
        m_state.prevPosition = rawPos;
        m_state.position = rawPos;

        m_state.prevWheel = raw.scrollWheelValue;
        m_state.wheel = raw.scrollWheelValue;

        m_initialized = true;
        return;
    }

    UpdateButton(raw, UpdateMode::Normal);

    m_state.prevPosition = m_state.position;
    m_state.position = ToPoint(raw.x, raw.y);

    m_state.prevWheel = m_state.wheel;
    m_state.wheel = raw.scrollWheelValue;
}

const MouseState& DXMouseInputProvider::GetState() const noexcept
{
    assert(m_initialized && "DXMouseInputProvider not initialized");
    return m_state;
}

//////////////////////////////////////////////////

#ifdef _WIN32
unique_ptr<IMouseInputProvider> CreateDXMouseInputProvider(HWND hwnd)
{
	return make_unique<DXMouseInputProvider>(hwnd);
}
#endif
