#pragma once

struct Task;
struct TaskContext;
struct CompiledTask;
class CommandList;

void ExecuteTaskImmediate(CommandList* cmd, const Task& task, TaskContext& ctx);

void ExecuteRenderPipeline(
	CommandList& cmd, 
	const vector<CompiledTask>& compiledTasks, 
	TaskContext& ctx); // 렌더링을 위해 컴파일된 태스크들을 일괄 순차 실행