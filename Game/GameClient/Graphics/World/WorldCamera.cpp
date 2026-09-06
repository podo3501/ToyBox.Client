#include "pch.h"
#include "WorldCamera.h"
#include "Core/Math/CameraMatrices.h"
#include <numbers>

static float DegToRad(float deg)
{
    return deg * std::numbers::pi_v<float> / 180.0f;
}

WorldCamera::~WorldCamera() = default;
WorldCamera::WorldCamera()
{
    SetNearFar(0.1f, 1000.f);
}

void WorldCamera::SetRotation(float pitch, float yaw)
{
    m_pitch = pitch;
    m_yaw = yaw;
    m_dirty = true;
}

void WorldCamera::SetFov(float fovDeg)
{
    m_fov = fovDeg;
    MarkProjDirty();
}

void WorldCamera::Move(const Core::Vector3& delta)
{
    m_position = m_position + delta;
    m_dirty = true;
}

void WorldCamera::MoveForward(float distance)
{
    UpdateIfNeeded(); // base가 private라 직접 못 부름 -> 아래 참고
    m_position = m_position + m_forward * distance;
    m_dirty = true;
}

void WorldCamera::MoveRight(float distance)
{
    UpdateIfNeeded();

    m_position = m_position + m_right * distance;
    m_dirty = true;
}

void WorldCamera::MoveUp(float distance)
{
    m_position.y += distance;
    m_dirty = true;
}

void WorldCamera::UpdateMatrices() const
{
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
}

Core::Matrix WorldCamera::BuildProjection(const Size& screenSize, const std::optional<Rect>& viewport) const
{
    float aspect;
    if (viewport.has_value())
        aspect = viewport->width / viewport->height;
    else
        aspect = static_cast<float>(screenSize.width) / static_cast<float>(screenSize.height);

    if (aspect != m_lastAspect || GetProjVersion() != m_lastProjVersion)
    {
        m_proj = Core::CreatePerspectiveFov(DegToRad(m_fov), aspect, m_nearZ, m_farZ);
        m_lastAspect = aspect;
        m_lastProjVersion = GetProjVersion();
    }
    return m_proj;
}