#include "pch.h"
#include "PendingLoadQueue.h"
#include "Resource/IPendingResource.h"

void PendingLoadQueue::Add(std::shared_ptr<IPendingResource> res)
{
    m_pendingLoads.push_back(std::move(res));
}

void PendingLoadQueue::Flush()
{
    for (auto it = m_pendingLoads.begin(); it != m_pendingLoads.end();)
    {
        auto& loadRes = *it;
        if (!loadRes->IsDependencyReady())
        {
            ++it;
            continue;
        }
        loadRes->MarkReady();
        it = m_pendingLoads.erase(it);
    }
}