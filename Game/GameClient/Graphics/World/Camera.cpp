#include "pch.h"
#include "Camera.h"
#include "Core/Math/CameraMatrices.h"
#include <numbers>

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

void Camera::Move(const Core::Vector3& delta)
{
    m_position = m_position + delta;
    m_dirty = true;
}

void Camera::MoveForward(float distance)
{
    UpdateIfNeeded();

    m_position = m_position + m_forward * distance;
    m_dirty = true;
}

void Camera::MoveRight(float distance)
{
    UpdateIfNeeded();

    m_position = m_position + m_right * distance;
    m_dirty = true;
}

void Camera::MoveUp(float distance)
{
    m_position.y += distance;
    m_dirty = true;
}

const Core::Matrix& Camera::GetView() const
{
    UpdateIfNeeded();
    return m_view;
}

const Core::Matrix& Camera::GetProj() const
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

CameraData Camera::BuildOrthographic(const Size& size, float nearZ, float farZ)
{
    CameraData data;
    data.view = Core::Matrix::Identity();
    data.proj = Core::Matrix::OrthographicOffCenter( 
        0.0f, 
        static_cast<float>(size.width),
        static_cast<float>(size.height),
        0.0f,  
        nearZ, farZ); // bottom, top : 화면 좌표계는 y가 아래로 갈수록 증가하므로 top/bottom을 뒤집음
    data.position = { 0.0f, 0.0f, 0.0f };

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

    m_forward.x = sinYaw * cosPitch;
    m_forward.y = sinPitch;
    m_forward.z = cosYaw * cosPitch;
    m_forward.Normalize();

    Core::Vector3 worldUp = { 0.0f, 1.0f, 0.0f };
    m_right = worldUp.Cross(m_forward);
    m_right.Normalize();

    m_up = m_forward.Cross(m_right);
    m_up.Normalize();

    Core::Vector3 target = m_position + m_forward;
    m_view = Core::CreateLookAt(m_position, target, worldUp);
    m_proj = Core::CreatePerspectiveFov(DegToRad(m_fov), m_aspect, m_nearZ, m_farZ);
}