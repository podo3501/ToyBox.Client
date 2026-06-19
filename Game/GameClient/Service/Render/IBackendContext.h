#pragma once
#include "GameClient/Graphics/RenderData/FrameData.h"
#include "GameClient/Service/Render/Repository/Mesh/IMeshProvider.h"
#include "GameClient/Service/Render/Repository/Material/IMaterialProvider.h"

struct IBackendContext
{
	virtual void SetFrameData(const FrameData& frameData) noexcept = 0;
	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) = 0;
	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) = 0;

	virtual IMeshProvider* GetMeshProvider() = 0;
	virtual IMaterialProvider* GetMaterialProvider() = 0;
};
