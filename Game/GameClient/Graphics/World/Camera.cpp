#include "pch.h"
#include "Camera.h"
#include "Core/Math/CameraMatrices.h"
#include <numbers>

namespace cm = Core::Math;

void Camera::SetRotation(float pitch, float yaw)
{
    m_pitch = pitch;
    m_yaw = yaw;
    m_dirty = true;
}

void Camera::SetFov(float fovDeg)
{
    m_fov = fovDeg;
    m_dirty = true;
}

void Camera::SetAspect(float aspect)
{
    m_aspect = aspect;
    m_dirty = true;
}

void Camera::SetNearFar(float nearZ, float farZ)
{
    m_nearZ = nearZ;
    m_farZ = farZ;
    m_dirty = true;
}

void Camera::Move(const cm::Vector3& delta)
{
    m_position = m_position + delta;
    m_dirty = true;
}

void Camera::MoveForward(float distance)
{
    cm::Vector3 forward;

    forward.x = cosf(m_pitch) * cosf(m_yaw);
    forward.y = sinf(m_pitch);
    forward.z = cosf(m_pitch) * sinf(m_yaw);

    m_position = m_position + forward * distance;
    m_dirty = true;
}

void Camera::MoveRight(float distance)
{
    cm::Vector3 forward;

    forward.x = cosf(m_pitch) * cosf(m_yaw);
    forward.y = sinf(m_pitch);
    forward.z = cosf(m_pitch) * sinf(m_yaw);

    cm::Vector3 up = { 0,1,0 };
    cm::Vector3 right = up.Cross(forward);

    m_position = m_position + right * distance;
    m_dirty = true;
}

void Camera::MoveUp(float distance)
{
    m_position.y += distance;
    m_dirty = true;
}

const cm::Matrix& Camera::GetView() const
{
    UpdateIfNeeded();
    return m_view;
}

const cm::Matrix& Camera::GetProj() const
{
    UpdateIfNeeded();
    return m_proj;
}

CameraData Camera::BuildCameraData() const
{
    UpdateIfNeeded();

    CameraData data;
    data.view = m_view;
    data.proj = m_proj;
    data.position = m_position;

    return data;
}

void Camera::UpdateIfNeeded() const
{
    if (!m_dirty)
        return;

    UpdateMatrices();
    m_dirty = false;
}

static float DegToRad(float deg)
{
    return deg * std::numbers::pi_v<float> / 180.0f;
}

void Camera::UpdateMatrices() const
{
    // Forward direction (Y-up, left-handed 기준 가정)
    float cosPitch = cosf(m_pitch);
    float sinPitch = sinf(m_pitch);
    float cosYaw = cosf(m_yaw);
    float sinYaw = sinf(m_yaw);

    cm::Vector3 forward;
    forward.x = sinYaw * cosPitch;
    forward.y = sinPitch;
    forward.z = cosYaw * cosPitch;

    cm::Vector3 target = m_position + forward;
    cm::Vector3 up = { 0.0f, 1.0f, 0.0f };

    m_view = cm::CreateLookAt(m_position, target, up);
    m_proj = cm::CreatePerspectiveFov(DegToRad(m_fov), m_aspect, m_nearZ, m_farZ);
}