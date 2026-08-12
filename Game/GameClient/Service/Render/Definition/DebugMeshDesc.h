#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

struct DebugMeshDesc
{
	Core::ResourceID resID;

	bool operator==(const DebugMeshDesc&) const = default;
	size_t GetHash() const { return Core::HashOf(resID); }
};