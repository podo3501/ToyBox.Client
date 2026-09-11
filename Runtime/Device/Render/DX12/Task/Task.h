#pragma once
#include "TaskHandle.h"
#include "Command/CommandType.h"
#include "Types/TaskContext.h"
#include "Types/TaskCommandLists.h"
#include <functional>

class CommandList;

using Microsoft::WRL::ComPtr;

struct Task
{
    std::string passName{};
    CommandType type{ CommandType::None };
    uint32_t numParallel{ 1 };

    std::vector<TaskHandle> dependencies; //앞에 Task에 의존하는지. Task의 시작지점을 알게 해 준다.
    std::function<void(TaskCommandLists, TaskContext&)> execute{ nullptr };
};