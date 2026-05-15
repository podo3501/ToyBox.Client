#pragma once
#include "Core/Math/Vector3.h"
#include "Core/Math/Matrix.h"

struct CameraData
{
    Core::Math::Matrix view;
    Core::Math::Matrix proj;
    Core::Math::Vector3 position;
};
