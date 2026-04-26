#include "pch.h"
#include "TaskScheduler.h"
#include "CommandScheduler.h"

TaskScheduler::~TaskScheduler() = default;
TaskScheduler::TaskScheduler(CommandScheduler* cmdScheduler)
    : m_cmdScheduler(cmdScheduler)
{}

TaskHandle TaskScheduler::AllocateHandle()
{
    return m_tasks.Emplace(Task{});
}

void TaskScheduler::Commit(TaskHandle handle, const TaskDesc& desc, std::shared_ptr<FrameResources> resources)
{
    Task* task = m_tasks.Find(handle);
    if (!task) return;

    task->desc = desc;
    task->context.resources = resources;
    for (auto& depHandle : desc.dependencies)
    {
        Task* depTask = m_tasks.Find(depHandle);
        if (depTask)
            depTask->dependents.push_back(handle);
    }

    assert(!task->committed);
    task->committed = true;
}

TaskHandle TaskScheduler::Enqueue(const TaskDesc& desc, shared_ptr<FrameResources> resources)
{
    Task task;
    task.desc = desc;
    task.context.resources = resources;

    auto taskHandle = m_tasks.Emplace(std::move(task));
    for (auto& depHandle : desc.dependencies) //역으로 의지하는 Task를 찾는다.
    {
        Task* depTask = m_tasks.Find(depHandle);
        if (depTask)
            depTask->dependents.push_back(taskHandle);
    }

    return taskHandle;
}

TaskHandle TaskScheduler::CreateTask(const TaskDesc& desc, std::shared_ptr<FrameResources> resources)
{
    Task task;
    task.desc = desc;
    task.context.resources = resources;
    return m_tasks.Emplace(std::move(task));
}

void TaskScheduler::Execute()
{
    std::vector<TaskHandle> toRemove;

    m_tasks.Visit([this, &toRemove](TaskHandle handle, Task& task) {
        if (!task.submitted)
        {
            if (AreDependenciesDone(task))
                ExecuteTask(task);
            return;
        }

        if (!task.finished && IsTaskFinished(task))
            task.finished = true;

        if (CanDeleteTask(task))
            toRemove.push_back(handle);
        });

    for (auto& handle : toRemove)
    {
        if (Task* task = m_tasks.Find(handle))
            RemoveTask(handle, *task);
    }
}

//void TaskScheduler::Execute() 
//{
//    for (auto handle : m_executionOrder) 
//    {
//        Task* task = Find(handle); 
//        if (!task) continue; 
//
//        if (AreDependenciesDone(*task)) // dependency 체크
//            ExecuteTask(*task); 
//    } 
//}

void TaskScheduler::ExecuteTask(Task& task)
{
    // CPU TASK
    if (task.desc.type == CommandType::None)
    {
        task.desc.cpuExecute(task.context); // no command list
        task.fenceValue = 0;
        task.submitted = true;
        return;
    }

    // GPU TASK
    auto cmd = m_cmdScheduler->Begin(task.desc.type);
    if (!cmd) return;

    task.desc.gpuExecute(*cmd, task.context);

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

bool TaskScheduler::CanDeleteTask(const Task& task)
{
    return task.finished && task.dependents.empty();
}

void TaskScheduler::RemoveTask(TaskHandle handle, Task& task)
{
    for (auto& depHandle : task.desc.dependencies)
    {
        if (Task* dep = m_tasks.Find(depHandle))
            std::erase(dep->dependents, handle);
    }

    m_tasks.Remove(handle);
}

void TaskScheduler::Cancel(TaskHandle handle)
{
    Task* task = m_tasks.Find(handle);
    if (!task) return;

    for (auto& depHandle : task->desc.dependencies)
    {
        if (Task* dep = m_tasks.Find(depHandle))
            std::erase(dep->dependents, handle);
    }

    for (auto& childHandle : task->dependents)
    {
        if (Task* child = m_tasks.Find(childHandle))
            std::erase(child->desc.dependencies, handle);
    }

    m_tasks.Remove(handle);
}

void TaskScheduler::Clear()
{
    std::vector<TaskHandle> toRemove;

    m_tasks.Visit([this, &toRemove](TaskHandle handle, const Task& task) {
        if (CanDeleteTask(task))
            toRemove.push_back(handle);
        });

    for (auto& handle : toRemove)
    {
        if (Task* task = m_tasks.Find(handle))
            RemoveTask(handle, *task);
    }
}
