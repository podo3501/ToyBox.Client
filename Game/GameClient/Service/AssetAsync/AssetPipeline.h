#pragma once
#include "Core/Foundation/ResourceID.h"
#include <vector>
#include <mutex>
#include <utility>
#include <unordered_map>
#include <optional>
#include <filesystem>
#include <queue>
#include <atomic>
#include <condition_variable>
#include "AssetAsyncTypes.h"

template<typename T>
class AssetStorage
{
public:
    AssetRequestID Push(T value)
    {
        const AssetRequestID id = ++m_nextID;
        m_entries.emplace(id, std::move(value));

        return id;
    }

    template<typename... Args>
    AssetRequestID Emplace(Args&&... args)
    {
        const AssetRequestID id = ++m_nextID;
        m_entries.emplace(id, T{ std::forward<Args>(args)... });

        return id;
    }

    void Insert(AssetRequestID id, T value)
    {
        auto [_, inserted] = m_entries.emplace(id, std::move(value));
        Assert(inserted); //id가 중복되어서는 안된다. result로 이 함수는 사용되는데 중복된다면 이전 아이디 값에 들고있던 로딩된 데이터가 날라간다.
    }

    std::optional<T> Take(AssetRequestID id)
    {
        auto it = m_entries.find(id);
        if (it == m_entries.end())
            return std::nullopt;

        T value = std::move(it->second);
        m_entries.erase(it);

        return value;
    }

    bool Contains(AssetRequestID id) const
    {
        return m_entries.find(id) != m_entries.end();
    }

    size_t Size() const { return m_entries.size(); }
    void Clear() { m_entries.clear(); }

private:
    std::unordered_map<AssetRequestID, T> m_entries;
    std::atomic<AssetRequestID> m_nextID{ InvalidAssetRequestID };
};

class AssetPipeline
{
public:
    void Shutdown()
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

    AssetRequestID PushRequest(AssetRequest req)
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);

        AssetRequestID id = m_requests.Emplace(std::move(req));
        m_pending.push(id);
       
        m_pendingCV.notify_one();

        return id;
    }

    bool WaitPopPending(AssetRequestID& outId)
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

    std::optional<AssetRequest> TakeRequest(AssetRequestID id)
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        return m_requests.Take(id);
    }

    void PushResult(AssetRequestID id, AssetPtr result)
    {
        std::lock_guard<std::mutex> lock(m_waitMutex);
        m_results.Insert(id, std::move(result));
        m_waitCV.notify_all();
    }

    std::optional<AssetPtr> TakeResult(AssetRequestID id)
    {
        std::lock_guard<std::mutex> lock(m_waitMutex);
        return m_results.Take(id);
    }

    AssetPtr Wait(AssetRequestID id)
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

    bool HasPendingWork() const
    {
        std::scoped_lock lock(m_pendingMutex, m_waitMutex);

        const bool hasPending = !m_pending.empty();
        const bool hasRequests = m_requests.Size() != 0;
        const bool hasResults = m_results.Size() != 0;

        return hasPending || hasRequests || hasResults;
    }

private:
    std::atomic<bool> m_shutdown{ false };

    // Pending 그룹 (pendingMutex로 일괄 보호)
    mutable std::mutex m_pendingMutex;
    AssetStorage<AssetRequest> m_requests;
    std::queue<AssetRequestID> m_pending;
    std::condition_variable m_pendingCV;

    // Wait/Result 그룹 (waitMutex로 일괄 보호)
    mutable std::mutex m_waitMutex;
    AssetStorage<AssetPtr> m_results;
    std::condition_variable m_waitCV;
};