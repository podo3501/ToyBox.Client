#include "pch.h"
#include "UICamera.h"
#include "Core/Math/CameraMatrices.h"

UICamera::~UICamera() = default;
UICamera::UICamera()
{
    SetNearFar(0.f, 1.f);
}

Core::Matrix UICamera::BuildProjection(const Size& screenSize, const std::optional<Rect>& viewport) const
{
    float width = m_width;
    float height = m_height;

    if (width <= 0.0f || height <= 0.0f)
    {
        if (viewport.has_value())
        {
            width = viewport->width;
            height = viewport->height;
        }
        else
        {
            width = static_cast<float>(screenSize.width);
            height = static_cast<float>(screenSize.height);
        }
    }

    if (width != m_lastWidth || height != m_lastHeight || GetProjVersion() != m_lastProjVersion)
    {
        m_proj = Core::Matrix::OrthographicOffCenter(0.f, width, height, 0.f, m_nearZ, m_farZ);
        m_lastWidth = width;
        m_lastHeight = height;
        m_lastProjVersion = GetProjVersion();
    }
    return m_proj;
}

void UICamera::SetOrthoSize(float width, float height) 
{
    m_width = width;
    m_height = height; 

    MarkProjDirty(); 
}

void UICamera::UpdateMatrices() const
{
    // UI는 회전 없음. 패닝이 필요해지면 여기서 m_position 반영.
    m_view = Core::Matrix::Identity();
}