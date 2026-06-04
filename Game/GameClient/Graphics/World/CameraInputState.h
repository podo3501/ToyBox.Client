#pragma once

struct CameraInputState
{
    float moveForward = 0.0f;
    float moveRight = 0.0f;
    float moveUp = 0.0f;

    float yawDelta = 0.0f;
    float pitchDelta = 0.0f;

    bool fastMove = false;
};