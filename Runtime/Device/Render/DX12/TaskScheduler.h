#pragma once
#include "Task.h"
#include "Core/Utils/Handle/HandlePool.h"

class CommandScheduler;

struct TaskEntry
{
    Task task;
    TaskContext context;
    std::vector<TaskHandle> dependents; //다른 Task가 나를 의존하고 있는지. 이게 없으면 지울때 뒤에 Task 생각안하고 바로 삭제되버림.

    bool submitted{ false };
    bool started{ false };
    bool finished{ false };
    uint64_t fenceValue{ 0 };
};

class TaskScheduler
{
public:
    TaskScheduler(CommandScheduler* cmdScheduler);
    ~TaskScheduler();

    TaskHandle AllocateHandle();
    void Submit(const std::vector<CompiledTask>& compiledTasks, 
        std::shared_ptr<ResourceContext> resources, std::shared_ptr<UploadContext> upload);
    void Execute();
    void Cancel(TaskHandle handle);
    void Clear();
    void SetExecutionOrder(const std::vector<TaskHandle>& ordered) { m_executionOrder = ordered; }

private:
    bool AreDependenciesDone(const TaskEntry& task);
    bool IsTaskFinished(const TaskEntry& task);
    void ExecuteTask(TaskEntry& task);
    bool CanDeleteTask(const TaskEntry& task);
    void RemoveTask(TaskHandle handle, TaskEntry& task);

private:
    CommandScheduler* m_cmdScheduler{ nullptr };
    HandlePool<TaskEntry, struct TaskTag> m_tasks;
    std::vector<TaskHandle> m_executionOrder;
    uint32_t m_nextId{ 1 };
};