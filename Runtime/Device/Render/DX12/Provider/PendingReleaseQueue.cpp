#include "pch.h"
#include "PendingReleaseQueue.h"
#include "Graph/TaskScheduler.h"

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

    m_taskScheduler.DeferRelease(std::move(m_pendingReleases));
}