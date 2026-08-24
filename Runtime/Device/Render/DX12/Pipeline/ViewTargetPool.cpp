#include "pch.h"
#include "ViewTargetPool.h"
#include "Resource/Internal/ViewTargetResource.h"
#include "Graph/Task.h"

ViewTargetPool::~ViewTargetPool() = default;
ViewTargetPool::ViewTargetPool(
    Device& device, 
    TaskScheduler& taskScheduler,
    DescriptorFactory& descFactory) noexcept :
    m_device{ device }, 
    m_descFactory{ descFactory },
    m_pendingRelease{ taskScheduler }
{}

ViewTargetResource& ViewTargetPool::Acquire(uint32_t id, const Size& requiredSize)
{
    auto it = m_views.find(id);
    if (it != m_views.end())
    {
        if (it->second->GetSize() == requiredSize)
            return *it->second;

        m_pendingRelease.Add(it->second); // 크기가 바뀜 -> 즉시 파괴하지 않고 pending release로 넘김 (GPU가 아직 쓰고 있을 수 있으므로)
        m_views.erase(it);
    }

    auto view = std::make_shared<ViewTargetResource>();
    view->Initialize(m_device, m_descFactory, requiredSize);

    auto [inserted, _] = m_views.emplace(id, std::move(view));
    return *inserted->second;
}

void ViewTargetPool::ApplyResourceBindings(ResourceContext& resCtx) const
{
    for (auto& [id, view] : m_views)
    {
        resCtx.Set(view->GetColorID(), view->GetColorResource());
        resCtx.Set(view->GetDepthID(), view->GetDepthResource());
    }
}

void ViewTargetPool::PruneUnused(const std::unordered_set<uint32_t>& activeViews)
{
    for (auto it = m_views.begin(); it != m_views.end(); )
    {
        if (!activeViews.contains(it->first))
        {
            m_pendingRelease.Add(it->second);
            it = m_views.erase(it);
        }
        else
            ++it;
    }
}

void ViewTargetPool::Update()
{
    m_pendingRelease.Flush();
}