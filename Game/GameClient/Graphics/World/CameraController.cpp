#include "pch.h"
#include "CameraController.h"
#include "Camera.h"

void CameraController::Update(const CameraInputState& input, Camera& camera, float deltaTime) const noexcept
{
    float speed = input.fastMove ? m_boostSpeed : m_moveSpeed;
    float moveAmount = speed * deltaTime;

    if (input.moveForward != 0.0f) camera.MoveForward(input.moveForward * moveAmount);
    if (input.moveRight != 0.0f) camera.MoveRight(input.moveRight * moveAmount);
    if (input.moveUp != 0.0f) camera.MoveUp(input.moveUp * moveAmount);

    float yaw = camera.GetYaw() + input.yawDelta * m_mouseSensitivity;
    float pitch = camera.GetPitch() + input.pitchDelta * m_mouseSensitivity;

    constexpr float limit = 1.55334306f; // 89 degree
    pitch = std::clamp(pitch, -limit, limit);
    camera.SetRotation(pitch, yaw);
}