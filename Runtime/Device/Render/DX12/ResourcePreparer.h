#pragma once
#include <d3d12.h>
#include "FenceTypes.h"

struct ID3D12GraphicsCommandList;
struct PendingTransition;

class ResourcePreparer
{
public:
    void Enqueue(const PendingTransition& p);
    void Process(ID3D12GraphicsCommandList* cmd, const CompletedFences& fences);

private:
    vector<PendingTransition> m_pendingTransitions;
};
