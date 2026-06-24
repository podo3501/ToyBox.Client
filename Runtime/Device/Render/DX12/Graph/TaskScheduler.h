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

    std::atomic<int> activeDependents{ 0 }; // 나를 의존하는 자식 노드들 중, 아직 해제(Remove)되지 않고 살아있는 자식들의 총 개수. 이게 0이 되면 자신도 해제된다.

    TaskEntry() = default;
    TaskEntry(const TaskEntry&) = delete;
    TaskEntry& operator=(const TaskEntry&) = delete;

    //atomic 변수 때문에 이동 및 대입 연산자 작성.
    TaskEntry(TaskEntry&& other) noexcept
        : task(std::move(other.task))
        , context(std::move(other.context))
        , dependents(std::move(other.dependents))
        , submitted(other.submitted)
        , started(other.started)
        , finished(other.finished)
        , fenceValue(other.fenceValue)
    {
        activeDependents.store(other.activeDependents.load(std::memory_order_relaxed), std::memory_order_relaxed);
    }

    TaskEntry& operator=(TaskEntry&& other) noexcept
    {
        if (this != &other)
        {
            task = std::move(other.task);
            context = std::move(other.context);
            dependents = std::move(other.dependents);
            submitted = other.submitted;
            started = other.started;
            finished = other.finished;
            fenceValue = other.fenceValue;

            activeDependents.store(other.activeDependents.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
        return *this;
    }
};

class TaskScheduler
{
public:
    TaskScheduler(CommandScheduler& cmdScheduler);
    ~TaskScheduler();

    TaskHandle AllocateHandle();
    void Submit(const std::vector<CompiledTask>& compiledTasks, std::shared_ptr<ResourceContext> resources);
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
    CommandScheduler& m_cmdScheduler;
    HandlePool<TaskEntry, struct TaskTag> m_tasks;
    std::vector<TaskHandle> m_executionOrder;
    uint32_t m_nextId{ 1 };
};