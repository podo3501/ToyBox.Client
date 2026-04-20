#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include "CommandType.h"
#include "TaskHandle.h"

class CommandList;

struct TaskDesc
{
    CommandType type;
    std::vector<TaskHandle> dependencies;
    std::function<void(CommandList&)> execute;
};

struct Task
{
    TaskDesc desc;

    bool submitted{ false };
    bool finished{ false };
    uint64_t fenceValue{ 0 };
};