#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "../Provider/PendingReleaseQueue.h"
#include <bitset>

struct ResourceContext;
class Device;
class TaskScheduler;
class DescriptorFactory;
class RGRenderIDAllocator;
class ViewTargetResource;

class ViewTargetPool
{
public:
    ~ViewTargetPool();
    ViewTargetPool() = delete;
    ViewTargetPool(Device& device, TaskScheduler& taskScheduler, DescriptorFactory& descFactory) noexcept;

    ViewTargetResource& Acquire(
        ViewID id, 
        RGRenderIDAllocator& idAllocator,
        const Size& requiredSize);
    void ApplyResourceBindings(ResourceContext& resources) const;
    void PruneUnused(const std::bitset<Core::EnumSize<ViewID>>& activeViews);
    void Update();

private:
    Device& m_device;
    DescriptorFactory& m_descFactory;
    PendingReleaseQueue m_pendingRelease;

    std::array<std::shared_ptr<ViewTargetResource>, Core::EnumSize<ViewID>> m_views{};
};
