#include "pch.h"
#include "Transform.h"
#include "Core/Math/Vector3.h"

namespace cm = Core::Math;

Transform::Transform() : 
    position(0, 0, 0),
    rotation(0, 0, 0),
    scale(1, 1, 1),
    m_dirty(true)
{}

Transform::Transform(
    const cm::Vector3& pos,
    const cm::Vector3& rot,
    const cm::Vector3& scale) :
    position(pos),
    rotation(rot),
    scale(scale),
    m_dirty(true)
{}

void Transform::SetPosition(const cm::Vector3& p)
{
    position = p;
    m_dirty = true;
}

void Transform::SetRotation(const cm::Vector3& r)
{
    rotation = r;
    m_dirty = true;
}

void Transform::SetScale(const cm::Vector3& s)
{
    scale = s;
    m_dirty = true;
}

void Transform::SetDirty()
{
    m_dirty = true;
}

const cm::Matrix& Transform::GetMatrix() const
{
    if (m_dirty)
    {
        RebuildMatrix();
    }
    return m_worldMatrix;
}

void Transform::RebuildMatrix() const
{
    cm::Matrix T = cm::Matrix::Translation(position.x, position.y, position.z);
    cm::Matrix Rx = cm::Matrix::RotationX(rotation.x);
    cm::Matrix Ry = cm::Matrix::RotationY(rotation.y);
    cm::Matrix Rz = cm::Matrix::RotationZ(rotation.z);
    cm::Matrix S = cm::Matrix::Scale(scale.x, scale.y, scale.z);

    m_worldMatrix = T * Rx * Ry * Rz * S;

    m_dirty = false;
}