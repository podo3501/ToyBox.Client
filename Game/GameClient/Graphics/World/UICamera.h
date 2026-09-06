#pragma once
#include "Camera.h"

class UICamera : public Camera
{
public:
    virtual ~UICamera();
    UICamera();
    virtual Core::Matrix BuildProjection(const Size& screenSize, const std::optional<Rect>& viewport) const override;
    void SetOrthoSize(float width, float height); //명시적으로 크기를 안 주면 BuildProjection이 screenSize/viewport로 자동 계산

protected:
    virtual void UpdateMatrices() const override;

private:
    float m_width{ 0.0f };
    float m_height{ 0.0f };

    mutable Core::Matrix m_proj;
    mutable float m_lastWidth{ -1.0f };
    mutable float m_lastHeight{ -1.0f };
    mutable uint32_t m_lastProjVersion{ 0xFFFFFFFF };
};