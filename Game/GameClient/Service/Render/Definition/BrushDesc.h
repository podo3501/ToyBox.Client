#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

struct BrushDesc
{
	Core::ResourceID texture;

	bool operator==(const BrushDesc&) const = default;
	size_t GetHash() const { return Core::HashOf(texture); }
};