#pragma once
#include "Core/Math/Vector3.h"
#include "Core/Math/Matrix.h"

struct DirectionalLightData
{
    Core::Vector3 direction;
    Core::Vector3 color;
    float intensity{ 0.f };
    Core::Matrix viewProj;
};