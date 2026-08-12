#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

struct MeshDesc
{
	Core::ResourceID resID;

	bool operator==(const MeshDesc&) const = default;
	size_t GetHash() const { return Core::HashOf(resID); }
};