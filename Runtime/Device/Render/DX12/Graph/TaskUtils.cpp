#include "pch.h"
#include "TaskUtils.h"
#include "Task.h"
#include "Command/CommandScheduler.h"

void ExecuteTaskImmediate(std::span<CommandList*> cmds, const Task& task, TaskContext& ctx)
{
    Assert(!cmds.empty());
    Assert(task.execute != nullptr);
    task.execute(cmds, ctx);
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
            ExecuteTaskImmediate(parallelCmds, task, ctx);
            current = cmdScheduler.EndParallel(parallelCmds);
            Assert(current); // pool 고갈 시 여기서 즉시 드러남 - 이전에 짚었던 이슈
        }
    }

    return current;
}