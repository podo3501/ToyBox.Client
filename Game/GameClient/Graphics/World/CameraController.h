#pragma once
#include "CameraInputState.h"

class WorldCamera;

class CameraController
{
public:
    void Update(const CameraInputState& input, WorldCamera& camera, float deltaTime) const noexcept;

    void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
    void SetBoostSpeed(float speed) { m_boostSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }

private:
    float m_moveSpeed{ 0.5f };
    float m_boostSpeed{ 2.0f };
    float m_mouseSensitivity{ 0.0025f };
};