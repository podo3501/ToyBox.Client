#pragma once
#include "Core/Math/Vector3.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"

class DirectionalLight
{
public:
    void SetDirection(const Core::Math::Vector3& dir) { m_direction = dir.NormalizedOr({ 0.0f, -1.0f, 0.0f }); }
    void SetColor(const Core::Math::Vector3& color) { m_color = color; }
    void SetIntensity(float intensity) { m_intensity = intensity; }
    DirectionalLightData BuildLightData() const;

    const Core::Math::Vector3& GetDirection() const { return m_direction; }
    const Core::Math::Vector3& GetColor() const { return m_color; }
    float GetIntensity() const { return m_intensity; }

private:
    Core::Math::Vector3 m_direction{ 0.0f, -1.0f, 0.0f };
    Core::Math::Vector3 m_color{ 1.0f, 1.0f, 1.0f };
    float m_intensity{ 1.0f };
};
