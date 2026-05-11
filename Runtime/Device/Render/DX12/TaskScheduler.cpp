#include "pch.h"
#include "TaskScheduler.h"
#include "CommandScheduler.h"

TaskScheduler::~TaskScheduler() = default;
TaskScheduler::TaskScheduler(CommandScheduler* cmdScheduler)
    : m_cmdScheduler(cmdScheduler)
{}

TaskHandle TaskScheduler::AllocateHandle()
{
    return m_tasks.Emplace(TaskEntry{});
}

void TaskScheduler::Submit(
    const std::vector<CompiledTask>& compiledTasks, 
    std::shared_ptr<ResourceContext> resources,
    std::shared_ptr<UploadContext> upload)
{
    std::unordered_map<uint32_t, TaskHandle> remap; //RenderGraph에서 만든 일시적인 handle을 실제 사용가능한 task handle로 바꾼다.
    for (auto& compiled : compiledTasks)
    {
        TaskHandle realHandle = AllocateHandle();
        remap[compiled.localId] = realHandle;
    }

    for (const auto& compiled : compiledTasks)
    {
        TaskHandle handle = remap[compiled.localId];
        TaskEntry* entry = m_tasks.Find(handle);
        if (!entry) continue;

        assert(!entry->submitted);

        entry->task = compiled.task;
        for (auto& depLocalId : compiled.dependencies)
            entry->task.dependencies.push_back(remap[depLocalId]);
        for (auto& depLocalId : compiled.dependents)
            entry->dependents.push_back(remap[depLocalId]);
        entry->context.resources = resources;
        entry->context.upload = upload;
        entry->submitted = true;
    }
}

void TaskScheduler::Execute()
{
    std::vector<TaskHandle> toRemove;

    m_tasks.Visit([this, &toRemove](TaskHandle handle, TaskEntry& entry) {
        if (!entry.started)
        {
            if (AreDependenciesDone(entry))
                ExecuteTask(entry);
            return;
        }

        if (!entry.finished && IsTaskFinished(entry))
            entry.finished = true;

        if (CanDeleteTask(entry))
            toRemove.push_back(handle);
        });

    for (auto& handle : toRemove)
    {
        if (TaskEntry* entry = m_tasks.Find(handle))
            RemoveTask(handle, *entry);
    }
}

//void TaskScheduler::Execute() 
//{
//    for (auto handle : m_executionOrder) 
//    {
//        Task* task = Find(handle); 
//        if (!task) continue; 
//
//        if (AreDependenciesDone(*task)) // dependency 
//            ExecuteTask(*task); 
//    } 
//}

void TaskScheduler::ExecuteTask(TaskEntry& entry)
{
    // CPU TASK
    if (entry.task.type == CommandType::None)
    {
        entry.task.cpuExecute(entry.context); // no command list
        entry.fenceValue = 0;
        entry.started = true;
        return;
    }

    // GPU TASK
    auto cmd = m_cmdScheduler->Begin(entry.task.type);
    if (!cmd) 
        return;

    entry.task.gpuExecute(*cmd, entry.context);
    entry.fenceValue = m_cmdScheduler->End();
    entry.started = true;
}

bool TaskScheduler::AreDependenciesDone(const TaskEntry& entry)
{
    for (auto& dep : entry.task.dependencies)
    {
        const TaskEntry* depEntry = m_tasks.Find(dep);
        if (!depEntry) return false;
        if (!depEntry->finished) return false;
    }

    return true;
}

bool TaskScheduler::IsTaskFinished(const TaskEntry& entry)
{
    if (!entry.started) return false;
    return m_cmdScheduler->IsFenceComplete(entry.task.type, entry.fenceValue);
}

bool TaskScheduler::CanDeleteTask(const TaskEntry& entry)
{
    return entry.finished && entry.dependents.empty();
}

void TaskScheduler::RemoveTask(TaskHandle handle, TaskEntry& entry)
{
    for (auto& depHandle : entry.task.dependencies)
    {
        if (TaskEntry* dep = m_tasks.Find(depHandle))
            std::erase(dep->dependents, handle);
    }

    m_tasks.Remove(handle);
}

void TaskScheduler::Cancel(TaskHandle handle)
{
    TaskEntry* entry = m_tasks.Find(handle);
    if (!entry) return;

    for (auto& depHandle : entry->task.dependencies)
    {
        if (TaskEntry* dep = m_tasks.Find(depHandle))
            std::erase(dep->dependents, handle);
    }

    for (auto& childHandle : entry->dependents)
    {
        if (TaskEntry* child = m_tasks.Find(childHandle))
            std::erase(child->task.dependencies, handle);
    }

    m_tasks.Remove(handle);
}

void TaskScheduler::Clear()
{
    std::vector<TaskHandle> toRemove;

    m_tasks.Visit([this, &toRemove](TaskHandle handle, const TaskEntry& entry) {
        if (CanDeleteTask(entry))
            toRemove.push_back(handle);
        });

    for (auto& handle : toRemove)
    {
        if (TaskEntry* entry = m_tasks.Find(handle))
            RemoveTask(handle, *entry);
    }
}
