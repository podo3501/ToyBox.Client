#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Graphics/RenderData/CameraData.h"

class Camera
{
public:
    virtual ~Camera();
    virtual Core::Matrix BuildProjection(const Size& screenSize, const std::optional<Rect>& viewport) const = 0;

    void SetPosition(const Core::Vector3& pos);
    const Core::Vector3& GetPosition() const { return m_position; }

    void SetNearFar(float nearZ, float farZ);
    float GetNearZ() const { return m_nearZ; }
    float GetFarZ() const { return m_farZ; }

    uint32_t GetProjVersion() const { return m_projVersion; }
    const Core::Matrix& GetView() const;

protected:
    virtual void UpdateMatrices() const = 0;

    void UpdateIfNeeded() const;
    void MarkProjDirty() { ++m_projVersion; }

    Core::Vector3 m_position{ 0.f ,0.f ,0.f };
    float m_nearZ{ 0.1f };
    float m_farZ{ 1000.f };
    uint32_t m_projVersion{ 0 };

    mutable Core::Matrix m_view{ Core::Matrix::Identity() };
    mutable bool m_dirty{ true };
};