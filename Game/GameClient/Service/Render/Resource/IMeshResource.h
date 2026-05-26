#pragma once
#include "GameClient/Service/Asset/Assets/MeshAsset.h"

struct IMeshResource
{
	virtual ~IMeshResource() = default;
	virtual bool IsReady() const noexcept = 0;
	virtual VertexFormat GetVertexFormat() const noexcept = 0;
};