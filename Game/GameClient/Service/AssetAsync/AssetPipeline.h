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

using AssetRequestID = uint64_t;
inline constexpr AssetRequestID InvalidAssetRequestID = 0;

struct AssetRequest
{
    Core::ResourceID resID;
    Core::TypeID type{ Core::InvalidTypeID };
};

template<typename T>
struct QueuedAsset
{
    AssetRequestID id{ InvalidAssetRequestID };
    T value;
};

template<typename TAsset>
struct AssetResult
{
    AssetRequestID id{ InvalidAssetRequestID };
    TAsset asset;
};

template<typename T>
class AssetQueue
{
public:
    AssetRequestID Push(T value)
    {
        std::lock_guard<std::mutex> lock(m_entriesMutex);

        const AssetRequestID id = ++m_nextID;
        m_entries.emplace(id, std::move(value));

        return id;
    }

    template<typename... Args>
    AssetRequestID Emplace(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(m_entriesMutex);

        const AssetRequestID id = ++m_nextID;
        m_entries.emplace(id, T{ std::forward<Args>(args)... });

        return id;
    }

    void Insert(AssetRequestID id, T value)
    {
        std::lock_guard<std::mutex> lock(m_entriesMutex);
        auto [_, inserted] = m_entries.emplace(id, std::move(value));
        Assert(inserted); //id가 중복되어서는 안된다. result로 이 함수는 사용되는데 중복된다면 이전 아이디 값에 들고있던 로딩된 데이터가 날라간다.
    }

    std::optional<T> Take(AssetRequestID id)
    {
        std::lock_guard<std::mutex> lock(m_entriesMutex);

        auto it = m_entries.find(id);
        if (it == m_entries.end())
            return std::nullopt;

        T value = std::move(it->second);
        m_entries.erase(it);

        return value;
    }

    bool Contains(AssetRequestID id) const
    {
        std::lock_guard<std::mutex> lock(m_entriesMutex);
        return m_entries.find(id) != m_entries.end();
    }

    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_entriesMutex);
        return m_entries.size();
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_entriesMutex);
        m_entries.clear();
    }

private:
    std::unordered_map<AssetRequestID, T> m_entries;
    mutable std::mutex m_entriesMutex;

    std::atomic<AssetRequestID> m_nextID{ InvalidAssetRequestID };
};

template<typename TRequest, typename TResult>
class AssetPipeline
{
public:
    AssetRequestID PushRequest(TRequest req)
    {
        AssetRequestID id = m_requests.Emplace(std::move(req));

        {
            std::lock_guard<std::mutex> lock(m_pendingMutex);
            m_pending.push(id);
        }

        return id;
    }

    bool TryPopPending(AssetRequestID& outId)
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        
        if (m_pending.empty())
            return false;

        outId = m_pending.front();
        m_pending.pop();
        return true;
    }

    std::optional<TRequest> TakeRequest(AssetRequestID id)
    {
        return m_requests.Take(id);
    }

    void PushResult(AssetRequestID id, TResult result)
    {
        m_results.Insert(id, std::move(result));
    }

    std::optional<TResult> TakeResult(AssetRequestID id)
    {
        return m_results.Take(id);
    }

    bool HasResult(AssetRequestID id)
    {
        return m_results.Contains(id);
    }

    bool HasPendingWork() const
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);

        const bool hasPending = !m_pending.empty();
        const bool hasRequests = m_requests.Size() != 0;
        const bool hasResults = m_results.Size() != 0;

        return hasPending || hasRequests || hasResults;
    }

private:
    AssetQueue<TRequest> m_requests;

    std::queue<AssetRequestID> m_pending;
    mutable std::mutex m_pendingMutex;

    AssetQueue<TResult> m_results;
};