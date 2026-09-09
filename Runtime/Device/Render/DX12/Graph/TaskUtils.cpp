#include "pch.h"
#include "TaskUtils.h"
#include "Task.h"

void ExecuteTaskImmediate(std::span<CommandList*> cmds, const Task& task, TaskContext& ctx)
{
    Assert(!cmds.empty());
    Assert(task.execute != nullptr);
    task.execute(cmds, ctx);
}

void ExecuteRenderPipeline(CommandList& cmd, const vector<CompiledTask>& compiledTasks, TaskContext& ctx)
{
    CommandList* cmds[] = { &cmd };

    for (auto& compiled : compiledTasks)
        ExecuteTaskImmediate(cmds, compiled.task, ctx); // 렌더링 루프에서는 항상 유효한 CommandList가 있으므로 주소(&cmd)를 넘겨줌.
}