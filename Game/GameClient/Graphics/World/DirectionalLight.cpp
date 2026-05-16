#include "pch.h"
#include "DirectionalLight.h"


DirectionalLightData DirectionalLight::BuildLightData() const
{
    DirectionalLightData data{};
    data.direction = m_direction;
    data.color = m_color;
    data.intensity = m_intensity;

    return data;
}