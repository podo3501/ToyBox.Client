#include "pch.h"
#include "DXMouseInputProvider.h"

bool DXMouseInputProvider::IsMouseButtonPressed(MouseButton button) const noexcept
{
    auto idx = static_cast<size_t>(button);
    if (idx >= m_inputStates.size()) return false;

    return m_inputStates[idx].IsPressed();
}


void DXMouseInputProvider::Update() noexcept
{
    auto dxState = m_mouse.GetState();

    m_inputStates[(int)MouseButton::Left].Update(dxState.leftButton);
    m_inputStates[(int)MouseButton::Right].Update(dxState.rightButton);
    m_inputStates[(int)MouseButton::Middle].Update(dxState.middleButton);
    m_inputStates[(int)MouseButton::X1].Update(dxState.xButton1);
    m_inputStates[(int)MouseButton::X2].Update(dxState.xButton2);
}

//////////////////////////////////////////////////

std::unique_ptr<IMouseInputProvider> CreateDXMouseInputProvider()
{
	return make_unique<DXMouseInputProvider>();
}