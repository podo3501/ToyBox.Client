#pragma once
#include "Constants.h"

namespace Core::Math
{
     constexpr float ToRadians(float degrees)
    {
        return degrees * (PI / 180.0f);
    }

    constexpr float ToDegrees(float radians)
    {
        return radians * (180.0f / PI);
    }
}