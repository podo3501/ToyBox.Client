#include "pch.h"
#include "PendingReleaseQueue.h"

PendingReleaseQueue::~PendingReleaseQueue() = default;
PendingReleaseQueue::PendingReleaseQueue(TaskScheduler& taskScheduler) noexcept :
    m_taskScheduler{ taskScheduler }
{}

void PendingReleaseQueue::Add(std::shared_ptr<IResource> res)
{
    if (!res) 
        return;

    m_pendingReleases.emplace_back(std::move(res));
}

void PendingReleaseQueue::Flush()
{
    if (m_pendingReleases.empty())
        return;

    Task releaseTask;
    releaseTask.passName = "ReleaseResource";
    releaseTask.cpuExecute = [resources = std::move(m_pendingReleases)](TaskContext&) {
        // Fence 이후 여기까지 오기만 하면 자동으로 Release됨.
        };
    m_taskScheduler.SubmitReleaseTask(releaseTask);
}