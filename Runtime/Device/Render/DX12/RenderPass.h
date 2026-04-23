#pragma once
#include <string>
#include <vector>
#include <functional>

#include "CommandType.h"
#include "Task.h"

class CommandList;

struct RGTexture;

struct RenderPass
{
    std::string name;

    std::vector<RGTexture> reads;
    std::vector<RGTexture> writes;

    CommandType type;

    std::function<void(CommandList&, TaskContext&)> execute;
    std::function<void(TaskContext&)> onComplete;
};