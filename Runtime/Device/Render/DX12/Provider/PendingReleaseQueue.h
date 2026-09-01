#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"

class TaskScheduler;

class PendingReleaseQueue
{
public:
    ~PendingReleaseQueue();
    PendingReleaseQueue() = delete;
    explicit PendingReleaseQueue(TaskScheduler& taskScheduler) noexcept;

    void Add(std::shared_ptr<IResource> res);
    void Flush();

private:
    TaskScheduler& m_taskScheduler;
    std::vector<std::shared_ptr<IResource>> m_pendingReleases;
};
