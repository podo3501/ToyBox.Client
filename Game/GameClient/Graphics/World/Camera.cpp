#include "pch.h"
#include "Camera.h"

Camera::~Camera() = default;

void Camera::SetPosition(const Core::Vector3& pos) 
{ 
    m_position = pos; 
    m_dirty = true; 
}

void Camera::SetNearFar(float nearZ, float farZ) 
{ 
    m_nearZ = nearZ; 
    m_farZ = farZ; 
    ++m_projVersion; 
}

const Core::Matrix& Camera::GetView() const
{
    UpdateIfNeeded();
    return m_view;
}

void Camera::UpdateIfNeeded() const
{
    if (!m_dirty) return;

    UpdateMatrices();
    m_dirty = false;
}