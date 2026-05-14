#pragma once
#include "Core/Math/Vector3.h"
#include "Core/Math/Matrix.h"

class Transform
{
public:
    Transform();
    Transform(const Core::Math::Vector3& pos,
        const Core::Math::Vector3& rot,
        const Core::Math::Vector3& scale);

    void SetPosition(const Core::Math::Vector3& p);
    void SetRotation(const Core::Math::Vector3& r);
    void SetScale(const Core::Math::Vector3& s);

    const Core::Math::Matrix& GetMatrix() const;
    void SetDirty();

private:
    void RebuildMatrix() const;

    mutable Core::Math::Matrix m_worldMatrix;
    mutable bool m_dirty;

    Core::Math::Vector3 position;
    Core::Math::Vector3 rotation; // radians (x,y,z)
    Core::Math::Vector3 scale;
};