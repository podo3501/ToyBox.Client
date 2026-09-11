#pragma once
#include <cstdint>
#include <limits>

using LocalTaskID = uint32_t;
static constexpr LocalTaskID InvalidLocalTaskID = std::numeric_limits<LocalTaskID>::max();