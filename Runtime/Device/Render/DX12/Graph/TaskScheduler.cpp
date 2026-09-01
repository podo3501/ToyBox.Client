#include "pch.h"
#include "TaskScheduler.h"
#include "TaskUtils.h"
#include "Command/CommandScheduler.h"

struct PendingResourceRelease
{
    FenceID waitFenceID{ InvalidFenceID };
    std::vector<std::shared_ptr<IResource>> resources;
};

TaskScheduler::~TaskScheduler() = default;
TaskScheduler::TaskScheduler(CommandScheduler& cmdScheduler) : 
    m_cmdScheduler{ cmdScheduler }
{}

TaskHandle TaskScheduler::AllocateHandle()
{
    return m_tasks.Emplace(TaskEntry{});
}

void TaskScheduler::SubmitTask(const std::vector<CompiledTask>& compiledTasks, std::shared_ptr<ResourceContext> resources)
{
    Assert(!m_draining.load(std::memory_order_relaxed)); //Drain() 도중 SubmitTask 호출

    std::unordered_map<LocalTaskID, TaskHandle> remap; //RenderGraph에서 만든 일시적인 handle을 실제 사용가능한 task handle로 바꾼다.
    for (auto& compiled : compiledTasks)
        remap[compiled.localId] = AllocateHandle();

    for (const auto& compiled : compiledTasks)
    {
        TaskHandle handle = remap[compiled.localId];
        TaskEntry* entry = m_tasks.Find(handle);
        Assert(entry);
        Assert(!entry->submitted);

        entry->task = compiled.task;
        entry->context.resources = resources;

        for (auto& depLocalId : compiled.dependencies)
            entry->task.dependencies.push_back(remap[depLocalId]);
        for (auto& depLocalId : compiled.dependents)
            entry->dependents.push_back(remap[depLocalId]);

        // (최적화코드) 자신이 파괴될 때만 부모의 카운트를 깎아주면 되므로 std::erase가 필요 없어짐.
        for (auto& depHandle : entry->task.dependencies)
        {
            if (TaskEntry* parent = m_tasks.Find(depHandle))
                parent->activeDependents.fetch_add(1, std::memory_order_relaxed); //부모들의 살아있는 자식 수 카운트를 1씩 증가.
        }
        
        entry->submitted = true;
    }
}

void TaskScheduler::DeferRelease(std::vector<std::shared_ptr<IResource>> resources)
{
    Assert(!m_draining.load(std::memory_order_relaxed)); //Drain() 도중 SubmitTask 호출

    auto queue = m_cmdScheduler.GetQueue(CommandType::Direct);

    PendingResourceRelease entry;
    entry.waitFenceID = queue->GetCurrentFence();
    entry.resources = std::move(resources);

    m_pendingReleases.push_back(std::move(entry));
}

void TaskScheduler::Execute()
{
    ProcessPendingReleases();

    std::vector<TaskHandle> toRemove;
    m_tasks.Visit([this, &toRemove](TaskHandle handle, TaskEntry& entry) {
        if (!entry.started)
        {
            if (!AreDependenciesDone(entry))
                return;

            if (!IsDirectFenceReady(entry)) //release task용
                return;

            ExecuteTask(entry);
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

void TaskScheduler::ExecuteTask(TaskEntry& entry)
{
    CommandList* cmd = nullptr;
    const bool isGpuTask = (entry.task.type != CommandType::None);

    if (isGpuTask)
    {
        cmd = m_cmdScheduler.Begin(entry.task.type);
        if (!cmd) 
            return;
    }

    ExecuteTaskImmediate(cmd, entry.task, entry.context);

    entry.fenceID = isGpuTask ? m_cmdScheduler.End() : 0;
    entry.started = true;
}

bool TaskScheduler::AreDependenciesDone(const TaskEntry& entry)
{
    for (auto& dep : entry.task.dependencies)
    {
        const TaskEntry* depEntry = m_tasks.Find(dep);
        Assert(depEntry); //task가 중간에 어디론가 사라졌다는 뜻.

        if (!depEntry->finished) return false;
    }

    return true;
}

bool TaskScheduler::IsDirectFenceReady(const TaskEntry& entry) const
{
    if (entry.waitFenceID == InvalidFenceID)
        return true;

    auto queue = m_cmdScheduler.GetQueue(CommandType::Direct);
    return queue->GetCompletedFence() >= entry.waitFenceID;
}

bool TaskScheduler::IsTaskFinished(const TaskEntry& entry)
{
    if (!entry.started) return false;
    if (entry.task.type == CommandType::None) //cpu task라면 fence 값을 비교해 볼 필요가 없다.
        return true;

    return m_cmdScheduler.IsFenceComplete(entry.task.type, entry.fenceID);
}

bool TaskScheduler::CanDeleteTask(const TaskEntry& entry)
{
    // 내 작업이 끝났고, 나를 물고 늘어지는 자식 태스크가 하나도 없을 때만 안전하게 파괴 가능
    return entry.finished && (entry.activeDependents.load(std::memory_order_relaxed) == 0);
}

void TaskScheduler::RemoveTask(TaskHandle handle, TaskEntry& entry)
{
    //찾아서 지우는 부분(erase)을 지우고 카운트를 감소시키다가 0이 되면 삭제하게 수정.
    for (auto& depHandle : entry.task.dependencies)
    {
        if (TaskEntry* parent = m_tasks.Find(depHandle))
            parent->activeDependents.fetch_sub(1, std::memory_order_relaxed); // 부모의 activeDependents가 0이 되면, 다음 프레임 루프 때 부모도 CanDeleteTask를 통과해 자동으로 삭제.
    }

    m_tasks.Remove(handle);
}

void TaskScheduler::ProcessPendingReleases()
{
    auto queue = m_cmdScheduler.GetQueue(CommandType::Direct);
    FenceID completed = queue->GetCompletedFence();

    std::erase_if(m_pendingReleases, [completed](const PendingResourceRelease& entry) {
        return completed >= entry.waitFenceID; // 소멸 시 shared_ptr<IResource> 자동 해제
        });
}

void TaskScheduler::Shutdown()
{
    m_draining.store(true, std::memory_order_relaxed); // 신규 제출 차단
    Drain(); // 모든 task가 정상적으로 실행되고 끝날 때까지 그냥 기다림
    m_draining.store(false, std::memory_order_relaxed);
}

void TaskScheduler::Cancel(TaskHandle handle)
{
    TaskEntry* entry = m_tasks.Find(handle);
    if (!entry) return;

    // 자신이 취소되므로 내 부모들에게서 나의 자식 지분을 제거.
    for (auto& depHandle : entry->task.dependencies)
    {
        if (TaskEntry* parent = m_tasks.Find(depHandle))
            parent->activeDependents.fetch_sub(1, std::memory_order_relaxed);
    }

    for (auto& childHandle : entry->dependents)
    {
        if (TaskEntry* child = m_tasks.Find(childHandle))
            std::erase(child->task.dependencies, handle); // 자식의 정방향 디펜던시에서 취소된 나를 지워주어 자식이 무한 대기에 빠지지 않게 함.
    }

    m_tasks.Remove(handle);
}

bool TaskScheduler::IsIdle() const noexcept
{
    return m_tasks.Empty() && m_pendingReleases.empty();
}

void TaskScheduler::Drain()
{
    while (!IsIdle())
    {
        Execute(); // 완료된 task 제거 + pending release 처리
        std::this_thread::yield();
    }
}