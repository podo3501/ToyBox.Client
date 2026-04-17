#include "pch.h"
#include "ResourcePreparer.h"
#include "CommandUtils.h"
#include "PendingTransition.h"

void ResourcePreparer::Enqueue(const PendingTransition& p)
{
    if (!p.resource)
        return;

    m_pendingTransitions.push_back(p);
}

void ResourcePreparer::Process(
    ID3D12GraphicsCommandList* cmd,
    const CompletedFences& fences)
{
    size_t write = 0;

    for (size_t read = 0; read < m_pendingTransitions.size(); ++read)
    {
        auto& e = m_pendingTransitions[read];

        if (fences.copy >= e.fence)
        {
            CommandUtils::Transition(cmd, e.resource, e.before, e.after);
            if (e.owner) 
                e.owner->OnReady(cmd);
        }
        else
            m_pendingTransitions[write++] = e;
    }

    m_pendingTransitions.resize(write);
}