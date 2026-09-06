#pragma once
#include "Camera.h"

class WorldCamera : public Camera
{
public:
    virtual ~WorldCamera();
    WorldCamera();
    virtual Core::Matrix BuildProjection(
        const Size& screenSize, 
        const std::optional<Rect>& viewport) const override;

    void SetRotation(float pitch, float yaw);
    void SetFov(float fovDeg);

    void Move(const Core::Vector3& delta);
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);

    Core::Vector3 GetForward() const { return m_forward; }
    Core::Vector3 GetRight() const { return m_right; }
    Core::Vector3 GetUp() const { return m_up; }

    float GetPitch() const { return m_pitch; }
    float GetYaw()   const { return m_yaw; }
    float GetFov() const { return m_fov; }

protected:
    virtual void UpdateMatrices() const override;

private:
    float m_pitch{ 0.0f };
    float m_yaw{ 0.0f };
    float m_fov{ 60.0f };

    mutable Core::Vector3 m_forward{ 0.0f, 0.0f, 1.0f };
    mutable Core::Vector3 m_right{ 1.0f, 0.0f, 0.0f };
    mutable Core::Vector3 m_up{ 0.0f, 1.0f, 0.0f };

    mutable Core::Matrix m_proj;
    mutable float m_lastAspect{ -1.0f };
    mutable uint32_t m_lastProjVersion{ 0xFFFFFFFF };
};