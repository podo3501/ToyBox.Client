#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

struct EnvironmentDesc
{
	Core::ResourceID resID; // .envmap 매니페스트 경로

	bool operator==(const EnvironmentDesc&) const = default;
	size_t GetHash() const { return Core::HashOf(resID); }
};