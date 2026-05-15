#pragma once

#include "Matrix.h"
#include "Vector3.h"

namespace Core::Math
{
    Matrix CreateLookAt(
        const Vector3& eye,
        const Vector3& target,
        const Vector3& up);

    Matrix CreatePerspectiveFov(
        float fovY,
        float aspect,
        float zn,
        float zf);
}
