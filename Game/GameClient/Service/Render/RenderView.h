#pragma once
#include "Core/Foundation/Geometry2D.h"

struct CameraData;
class Camera;
class RepositoryContainer;

enum class ViewType : uint8_t { None, Scene, Overlay };

class RenderView
{
public:
	virtual ~RenderView();
	RenderView(const RenderView&) = delete;
	RenderView& operator=(const RenderView&) = delete;
	RenderView(RenderView&&) = delete;
	RenderView& operator=(RenderView&&) = delete;
	virtual bool IsEmpty() const = 0;

	ViewType Type() const { return m_type; }

protected:
	RenderView(ViewType type, RepositoryContainer& repositories);

	CameraData BuildCameraData(
		const Camera& camera, 
		const std::optional<Rect>& viewport, 
		const Size& screenSize);

	ViewType m_type{ ViewType::None };
	RepositoryContainer& m_repositories;
};
