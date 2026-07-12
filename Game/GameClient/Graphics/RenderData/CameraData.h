#pragma once
#include "Core/Math/Vector3.h"
#include "Core/Math/Matrix.h"

struct CameraData
{
    Core::Matrix view;
    Core::Matrix proj;
    Core::Vector3 position;
};
