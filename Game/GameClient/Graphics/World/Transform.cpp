#include "pch.h"
#include "Transform.h"
#include "Core/Math/Vector3.h"

Transform::Transform() : 
    position(0, 0, 0),
    rotation(0, 0, 0),
    scale(1, 1, 1),
    m_dirty(true)
{}

Transform::Transform(
    const Core::Vector3& pos,
    const Core::Vector3& rot,
    const Core::Vector3& scale) :
    position(pos),
    rotation(rot),
    scale(scale),
    m_dirty(true)
{}

void Transform::SetPosition(const Core::Vector3& p)
{
    position = p;
    m_dirty = true;
}

void Transform::SetRotation(const Core::Vector3& r)
{
    rotation = r;
    m_dirty = true;
}

void Transform::SetScale(const Core::Vector3& s)
{
    scale = s;
    m_dirty = true;
}

void Transform::SetDirty()
{
    m_dirty = true;
}

const Core::Matrix& Transform::GetMatrix() const
{
    if (m_dirty)
    {
        RebuildMatrix();
    }
    return m_worldMatrix;
}

void Transform::RebuildMatrix() const
{
    Core::Matrix T = Core::Matrix::Translation(position.x, position.y, position.z);
    Core::Matrix Rx = Core::Matrix::RotationX(rotation.x);
    Core::Matrix Ry = Core::Matrix::RotationY(rotation.y);
    Core::Matrix Rz = Core::Matrix::RotationZ(rotation.z);
    Core::Matrix S = Core::Matrix::Scale(scale.x, scale.y, scale.z);

    m_worldMatrix = S * Rx * Ry * Rz * T;

    m_dirty = false;
}