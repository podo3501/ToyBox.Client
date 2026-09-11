#pragma once
#include "Task.h"
#include "Types/LocalTaskID.h"
#include <vector>
#include <cstdint>

struct CompiledTask //RenderGraph에서 pass를 가지고 계산해서 tasks로 만든 결과물.
{
    LocalTaskID localId{ InvalidLocalTaskID };
    Task task{};
    std::vector<uint32_t> dependencies;
    std::vector<uint32_t> dependents;
};
