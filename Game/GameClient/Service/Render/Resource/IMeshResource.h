#pragma once
#include "GameClient/Asset/MeshAsset.h"

struct IMeshResource
{
	virtual ~IMeshResource() = default;
	virtual bool IsReady() const noexcept = 0;
	virtual VertexFormat GetVertexFormat() const noexcept = 0;
};