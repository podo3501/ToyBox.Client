#include "pch.h"
#include "CameraInputMapper.h"
#include "Service/Input/InputService.h"

CameraInputState BuildCameraInput(const InputService& input)
{
    CameraInputState state;

    if (input.IsKeyHeld(KeyCode::W)) 
        state.moveForward += 1.0f;
    if (input.IsKeyHeld(KeyCode::S))
        state.moveForward -= 1.0f;
    if (input.IsKeyHeld(KeyCode::D))
        state.moveRight += 1.0f;
    if (input.IsKeyHeld(KeyCode::A))
        state.moveRight -= 1.0f;
    if (input.IsKeyHeld(KeyCode::E)) 
        state.moveUp += 1.0f;
    if (input.IsKeyHeld(KeyCode::Q)) 
        state.moveUp -= 1.0f;
    state.fastMove = input.IsKeyHeld(KeyCode::LeftShift);

    if (input.IsMouseButtonHeld(MouseButton::Left))
    {
        auto mouse = input.GetMouseState();
        int deltaX = mouse.position.x - mouse.prevPosition.x;
        int deltaY = mouse.position.y - mouse.prevPosition.y;

        state.yawDelta = static_cast<float>(deltaX);
        state.pitchDelta = static_cast<float>(-deltaY);
    }

    return state;
}