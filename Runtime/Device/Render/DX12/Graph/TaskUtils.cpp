#include "pch.h"
#include "TaskUtils.h"
#include "Task.h"

void ExecuteTaskImmediate(CommandList* cmd, const Task& task, TaskContext& ctx)
{
    if (task.type == CommandType::None)
    {
        Assert(task.cpuExecute != nullptr); // CPU 전용 태스크(None)인데 cpuExecute가 등록되지 않음.
        Assert(task.gpuExecute == nullptr); // CPU 전용 태스크(None)에 gpuExecute가 등록되었음.
    }
    else
    {
        Assert(cmd);
        Assert(task.gpuExecute != nullptr); // GPU 태스크인데 gpuExecute가 등록되지 않음.
    }

    if (task.cpuExecute)
        task.cpuExecute(ctx);

    if (task.gpuExecute)
        task.gpuExecute(*cmd, ctx);
}

void ExecuteRenderPipeline(CommandList& cmd, const vector<CompiledTask>& compiledTasks, TaskContext& ctx)
{
    for (auto& compiled : compiledTasks)
        ExecuteTaskImmediate(&cmd, compiled.task, ctx); // 렌더링 루프에서는 항상 유효한 CommandList가 있으므로 주소(&cmd)를 넘겨줌.
}