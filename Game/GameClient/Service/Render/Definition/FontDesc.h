#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

struct FontDesc
{
	Core::ResourceID resID; // .ttf 파일 경로

	bool operator==(const FontDesc&) const = default;
	size_t GetHash() const { return Core::HashOf(resID); }
};