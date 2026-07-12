#pragma once

namespace Core {
    struct Vector3;
    struct Vector4;
    class Matrix;
}

DirectX::XMFLOAT3 ToXMFLOAT3(const Core::Vector3& vec);
DirectX::XMFLOAT4 ToXMFLOAT4(const Core::Vector4& vec);
XMMATRIX ToDXMatrix(const Core::Matrix& m);