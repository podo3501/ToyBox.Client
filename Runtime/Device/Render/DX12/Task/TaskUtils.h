#pragma once

struct Task;
struct TaskContext;
struct CompiledTask;
class CommandList;
class CommandScheduler;

void ExecuteTaskImmediate(
	std::span<CommandList*> cmds, 
	const Task& task, TaskContext& ctx);

CommandList* ExecuteRenderPipeline(
	CommandList* cmd,
	CommandScheduler& cmdScheduler,
	const vector<CompiledTask>& compiledTasks, 
	TaskContext& ctx); // 렌더링을 위해 컴파일된 태스크들을 일괄 순차 실행