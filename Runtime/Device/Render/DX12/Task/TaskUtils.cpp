#include "pch.h"
#include "TaskUtils.h"
#include "CompiledTask.h"
#include "Command/CommandScheduler.h"

void ExecuteTaskImmediate(std::span<CommandList*> cmds, const Task& task, TaskContext& ctx)
{
    Assert(!cmds.empty());
    Assert(task.execute != nullptr);
    task.execute(TaskCommandLists{ cmds }, ctx);
}

CommandList* ExecuteRenderPipeline(
    CommandList* cmd, 
    CommandScheduler& cmdScheduler,
    const vector<CompiledTask>& compiledTasks, 
    TaskContext& ctx)
{
    CommandList* current = cmd;

    for (auto& compiled : compiledTasks)
    {
        auto& task = compiled.task;

        if (compiled.task.numParallel <= 1)
        {
            CommandList* cmds[] = { current };
            ExecuteTaskImmediate(cmds, task, ctx); // 렌더링 루프에서는 항상 유효한 CommandList가 있으므로 주소(&cmd)를 넘겨줌.
        }
        else
        {
            auto parallelCmds = cmdScheduler.BeginParallel(task.numParallel);
            if (parallelCmds.empty())
                return nullptr;

            ExecuteTaskImmediate(parallelCmds, task, ctx);
            current = cmdScheduler.EndParallel(parallelCmds);
            if (!current) // 후속 primary cmd를 못 구함
                return nullptr;
        }
    }

    return current;
}