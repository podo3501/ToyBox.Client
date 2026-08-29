#pragma once
#include <cstdint>
#include <limits>

namespace Core
{
	using Index = uint32_t;
	static constexpr Index InvalidIndex = std::numeric_limits<Index>::max();
}