#include "pch.h"
#include "AssetAsyncLoader.h"

AssetRequestID AssetAsyncLoader::PushRequest(AssetRequest req)
{
    std::lock_guard<std::mutex> lock(m_pendingMutex);

    AssetRequestID id = m_requests.Emplace(std::move(req));
    m_pending.push(id);

    m_pendingCV.notify_one();

    return id;
}

void AssetAsyncLoader::Shutdown()
{
    m_shutdown.store(true);

    {
        std::lock_guard lock(m_pendingMutex);
        m_pendingCV.notify_all();
    }

    {
        std::lock_guard lock(m_waitMutex);
        m_waitCV.notify_all();
    }

}

bool AssetAsyncLoader::WaitPopPending(AssetRequestID& outId)
{
    std::unique_lock lock(m_pendingMutex);

    m_pendingCV.wait(lock, [&] {
        return m_shutdown || !m_pending.empty(); //조건이 참이면 깨어난다. 람다로 predicate를 넣어놓은건 허상 깸? 이 있기 때문.
        });

    if (m_shutdown)
        return false;

    outId = m_pending.front();
    m_pending.pop();

    return true;
}

std::optional<AssetRequest> AssetAsyncLoader::TakeRequest(AssetRequestID id)
{
    std::lock_guard<std::mutex> lock(m_pendingMutex);
    return m_requests.Take(id);
}

void AssetAsyncLoader::PushResult(AssetRequestID id, AssetPtr result)
{
    std::lock_guard<std::mutex> lock(m_waitMutex);
    m_results.Insert(id, std::move(result));
    m_waitCV.notify_all();
}

AssetPtr AssetAsyncLoader::TakeResult(AssetRequestID id)
{
    std::lock_guard<std::mutex> lock(m_waitMutex);
    auto result = m_results.Take(id);
    return result.value_or(nullptr);
}

AssetPtr AssetAsyncLoader::Wait(AssetRequestID id)
{
    std::unique_lock lock(m_waitMutex);

    m_waitCV.wait(lock, [&] {
        return m_results.Contains(id) || m_shutdown.load();
        });

    if (m_shutdown.load())
        return nullptr;

    auto result = m_results.Take(id);
    Assert(result.has_value());
    return std::move(*result);
}

bool AssetAsyncLoader::HasPendingWork() const
{
    std::scoped_lock lock(m_pendingMutex);

    const bool hasPending = !m_pending.empty();
    const bool hasRequests = m_requests.Size() != 0;

    return hasPending || hasRequests;
}