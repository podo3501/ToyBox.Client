#pragma once
#include <d3d12.h>
#include "FenceTypes.h"

struct PendingTransition;
class CommandList;

class ResourcePreparer
{
public:
    void Enqueue(const PendingTransition& p);
    void Process(CommandList& cmd, const QueueFences& fences);

private:
    vector<PendingTransition> m_pendingTransitions;
};
