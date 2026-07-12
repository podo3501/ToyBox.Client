#pragma once
#include "Core/Math/Vector3.h"
#include "Core/Math/Matrix.h"

class Transform
{
public:
    Transform();
    Transform(const Core::Vector3& pos,
        const Core::Vector3& rot,
        const Core::Vector3& scale);

    void SetPosition(const Core::Vector3& p);
    void SetRotation(const Core::Vector3& r);
    void SetScale(const Core::Vector3& s);

    const Core::Matrix& GetMatrix() const;
    void SetDirty();

private:
    void RebuildMatrix() const;

    mutable Core::Matrix m_worldMatrix;
    mutable bool m_dirty;

    Core::Vector3 position;
    Core::Vector3 rotation; // radians (x,y,z)
    Core::Vector3 scale;
};