#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "../Provider/PendingReleaseQueue.h"
#include <unordered_set>

class Device;
class TaskScheduler;
class DescriptorFactory;
class ViewTargetResource;

class ViewTargetPool
{
public:
    ~ViewTargetPool();
    ViewTargetPool() = delete;
    ViewTargetPool(Device& device, TaskScheduler& taskScheduler, DescriptorFactory& descFactory) noexcept;

    ViewTargetResource& Acquire(uint32_t id, const Size& requiredSize);
    void PruneUnused(const std::unordered_set<uint32_t>& activeViews);
    void Update();

private:
    Device& m_device;
    DescriptorFactory& m_descFactory;
    PendingReleaseQueue m_pendingRelease;

    std::unordered_map<uint32_t, std::shared_ptr<ViewTargetResource>> m_views;
};
