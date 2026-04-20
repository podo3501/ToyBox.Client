#pragma once
#include "Task.h"
#include "Core/Utils/Handle/HandlePool.h"

class CommandScheduler;

class TaskScheduler
{
public:
    TaskScheduler(CommandScheduler* cmdScheduler);
    ~TaskScheduler();

    TaskHandle Enqueue(const TaskDesc& desc);
    void Execute();
    void Clear();

private:
    bool AreDependenciesDone(const Task& task);
    bool IsTaskFinished(const Task& task);
    void ExecuteTask(Task& task);

private:
    CommandScheduler* m_cmdScheduler{ nullptr };
    HandlePool<Task, struct TaskTag> m_tasks;
    uint32_t m_nextId{ 1 };
};