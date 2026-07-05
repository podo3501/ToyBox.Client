#pragma once
#include "Graph/TaskScheduler.h"

struct IResource;
class ResourceReleaseBuilder
{
public:
	~ResourceReleaseBuilder();
	ResourceReleaseBuilder() = delete;
	explicit ResourceReleaseBuilder(TaskScheduler& taskScheduler) noexcept;

public:
    template <typename T>
    void ReleaseResources(std::vector<std::shared_ptr<T>> resources)
    {
        static_assert(std::is_base_of_v<IResource, T>, "T must derive from IResource");

        Task releaseTask;
        releaseTask.passName = "ReleaseResource";
        releaseTask.cpuExecute = [resources = std::move(resources)](TaskContext&) {
            // Fence 이후 여기까지 오기만 하면 자동으로 Release됨.
            };

        m_taskScheduler.SubmitReleaseTask(releaseTask);
    }

private:
	TaskScheduler& m_taskScheduler;
};
