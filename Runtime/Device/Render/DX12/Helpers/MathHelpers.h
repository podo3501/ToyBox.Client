#pragma once

namespace Core::Math {
    struct Vector3;
    class Matrix;
}

XMMATRIX ToDXMatrix(const Core::Math::Matrix& m);
DirectX::XMFLOAT3 ToXMFLOAT3(const Core::Math::Vector3& vec);