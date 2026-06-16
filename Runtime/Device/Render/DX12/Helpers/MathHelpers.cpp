#include "pch.h"
#include "MathHelpers.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector3.h"

XMMATRIX ToDXMatrix(const Core::Math::Matrix& m)
{
    return XMMATRIX(
        m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3],
        m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3],
        m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3],
        m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]);
}

DirectX::XMFLOAT3 ToXMFLOAT3(const Core::Math::Vector3& vec)
{
    return DirectX::XMFLOAT3(vec.x, vec.y, vec.z);
}