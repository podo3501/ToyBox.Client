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

ViewTargetResource& ViewTargetPool::Acquire(ViewID id, const Size& requiredSize)
{
    auto& view = m_views[Core::ToIndex(id)];
    if (view)
    {
        if (view->GetSize() == requiredSize)
            return *view;

        m_pendingRelease.Add(view);
        view.reset();
    }

    view = std::make_shared<ViewTargetResource>();
    view->Initialize(m_device, m_descFactory, requiredSize);

    return *view;
}

void ViewTargetPool::ApplyResourceBindings(ResourceContext& resCtx) const
{
    for (const auto& view : m_views)
    {
        if (!view) continue;
        
        resCtx.Set(view->GetColorID(), view->GetColorResource());
        resCtx.Set(view->GetDepthID(), view->GetDepthResource());
    }
}

void ViewTargetPool::PruneUnused(const std::bitset<Core::EnumSize<ViewID>>& activeViews)
{
    for (size_t i = 0; i < m_views.size(); ++i)
    {
        auto& view = m_views[i];
        if (!view)
            continue;

        if (activeViews.test(i))
            continue;

        m_pendingRelease.Add(view);
        view.reset();
    }
}

void ViewTargetPool::Update()
{
    m_pendingRelease.Flush();
}