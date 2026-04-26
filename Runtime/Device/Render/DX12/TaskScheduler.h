#pragma once
#include "Task.h"
#include "Core/Utils/Handle/HandlePool.h"

class CommandScheduler;

class TaskScheduler
{
public:
    TaskScheduler(CommandScheduler* cmdScheduler);
    ~TaskScheduler();

    TaskHandle AllocateHandle();
    void Commit(TaskHandle handle, const TaskDesc& desc, std::shared_ptr<FrameResources> resources);
    TaskHandle Enqueue(const TaskDesc& desc, shared_ptr<FrameResources> resources);
    TaskHandle CreateTask(const TaskDesc& desc, std::shared_ptr<FrameResources> resources);
    void Execute();
    void Cancel(TaskHandle handle);
    void Clear();
    Task* Find(TaskHandle h) { return m_tasks.Find(h); }
    void SetExecutionOrder(const std::vector<TaskHandle>& ordered) { m_executionOrder = ordered; }

private:
    bool AreDependenciesDone(const Task& task);
    bool IsTaskFinished(const Task& task);
    void ExecuteTask(Task& task);
    bool CanDeleteTask(const Task& task);
    void RemoveTask(TaskHandle handle, Task& task);

private:
    CommandScheduler* m_cmdScheduler{ nullptr };
    HandlePool<Task, struct TaskTag> m_tasks;
    std::vector<TaskHandle> m_executionOrder;
    uint32_t m_nextId{ 1 };
};