#pragma once
#include "GameClient/Graphics/RenderData/CameraData.h"

class Camera
{
public:
    Camera() = default;

    void SetPosition(const Core::Math::Vector3& pos) { m_position = pos; m_dirty = true; }
    const Core::Math::Vector3& GetPosition() const { return m_position; }

    void SetRotation(float pitch, float yaw);
    
    float GetPitch() const { return m_pitch; }
    float GetYaw() const { return m_yaw; }

    void SetFov(float fovDeg);
    void SetAspect(float aspect);
    void SetNearFar(float nearZ, float farZ);

    void Move(const Core::Math::Vector3& delta);
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);

    const Core::Math::Matrix& GetView() const;
    const Core::Math::Matrix& GetProj() const;
    CameraData BuildCameraData() const;

private:
    void UpdateIfNeeded() const;
    void UpdateMatrices() const;

    Core::Math::Vector3 m_position = { 0,0,0 };

    float m_pitch = 0.0f;
    float m_yaw = 0.0f;

    float m_fov = 60.0f;
    float m_aspect = 16.0f / 9.0f;
    float m_nearZ = 0.1f;
    float m_farZ = 1000.0f;

    mutable Core::Math::Matrix m_view; //cached
    mutable Core::Math::Matrix m_proj; //cached
    mutable bool m_dirty = true;
};