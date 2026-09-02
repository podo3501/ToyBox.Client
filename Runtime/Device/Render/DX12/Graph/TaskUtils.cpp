#include "pch.h"
#include "TaskUtils.h"
#include "Task.h"

void ExecuteTaskImmediate(CommandList* cmd, const Task& task, TaskContext& ctx)
{
    Assert(cmd);
    Assert(task.execute != nullptr);

    task.execute(*cmd, ctx);
}

void ExecuteRenderPipeline(CommandList& cmd, const vector<CompiledTask>& compiledTasks, TaskContext& ctx)
{
    for (auto& compiled : compiledTasks)
        ExecuteTaskImmediate(&cmd, compiled.task, ctx); // 렌더링 루프에서는 항상 유효한 CommandList가 있으므로 주소(&cmd)를 넘겨줌.
}