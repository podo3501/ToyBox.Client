#pragma once
#include "Core/Math/Vector3.h"
#include "Core/Math/Matrix.h"

struct DirectionalLightData
{
    Core::Math::Vector3 direction;
    Core::Math::Vector3 color;
    float intensity{ 0.f };
    Core::Math::Matrix viewProj;
};