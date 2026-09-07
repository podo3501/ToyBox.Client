#include "pch.h"
#include "RenderView.h"
#include "Graphics/World/Camera.h"

RenderView::~RenderView() = default;
RenderView::RenderView(ViewType type, RepositoryContainer& repositories) :
    m_type{ type },
    m_repositories{ repositories }
{}

CameraData RenderView::BuildCameraData(const Camera& camera, const std::optional<Rect>& viewport, const Size& screenSize)
{
    CameraData data;
    data.view = camera.GetView();
    data.position = camera.GetPosition();
    data.proj = camera.BuildProjection(screenSize, viewport);

    return data;
}