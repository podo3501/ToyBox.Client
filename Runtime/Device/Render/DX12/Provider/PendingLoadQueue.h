#pragma once

class IPendingResource;

class PendingLoadQueue
{
public:
    void Add(std::shared_ptr<IPendingResource> res);
    void Flush();

private:
    std::vector<std::shared_ptr<IPendingResource>> m_pendingLoads;
};