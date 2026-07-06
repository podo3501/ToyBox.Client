#pragma once

namespace Core::Math {
    struct Vector3;
    struct Vector4;
    class Matrix;
}

DirectX::XMFLOAT3 ToXMFLOAT3(const Core::Math::Vector3& vec);
DirectX::XMFLOAT4 ToXMFLOAT4(const Core::Math::Vector4& vec);
XMMATRIX ToDXMatrix(const Core::Math::Matrix& m);