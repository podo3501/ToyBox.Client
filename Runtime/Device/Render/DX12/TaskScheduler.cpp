#include "pch.h"
#include "TaskScheduler.h"
#include "CommandScheduler.h"

TaskScheduler::~TaskScheduler() = default;
TaskScheduler::TaskScheduler(CommandScheduler* cmdScheduler)
    : m_cmdScheduler(cmdScheduler)
{}

TaskHandle TaskScheduler::Enqueue(const TaskDesc& desc)
{
    Task task;
    task.desc = desc;
    return m_tasks.Emplace(std::move(task));
}

void TaskScheduler::Execute()
{
    m_tasks.Visit([this](TaskHandle, Task& task) {
        if (!task.submitted)
        {
            if (AreDependenciesDone(task))
                ExecuteTask(task);
            return;
        }

        if (!task.finished && IsTaskFinished(task))
            task.finished = true;
        });
}

void TaskScheduler::ExecuteTask(Task& task)
{
    auto cmd = m_cmdScheduler->Begin(task.desc.type);
    if (!cmd) return;

    task.desc.execute(*cmd);

    task.fenceValue = m_cmdScheduler->End();
    task.submitted = true;
}

bool TaskScheduler::AreDependenciesDone(const Task& task)
{
    for (auto& dep : task.desc.dependencies)
    {
        const Task* depTask = m_tasks.Find(dep);
        if (!depTask) return false;
        if (!depTask->finished) return false;
    }

    return true;
}

bool TaskScheduler::IsTaskFinished(const Task& task)
{
    if (!task.submitted) return false;
    return m_cmdScheduler->IsFenceComplete(task.desc.type, task.fenceValue);
}

void TaskScheduler::Clear()
{
    m_tasks.Clear();
}
