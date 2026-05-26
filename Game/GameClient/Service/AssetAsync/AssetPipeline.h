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
    std::filesystem::path path;
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
        std::lock_guard<std::mutex> lock(m_mutex);

        const AssetRequestID id = ++m_nextID;
        m_map.emplace(id, std::move(value));

        return id;
    }

    template<typename... Args>
    AssetRequestID Emplace(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        const AssetRequestID id = ++m_nextID;
        m_map.emplace(id, T{ std::forward<Args>(args)... });

        return id;
    }

    std::optional<T> Take(AssetRequestID id)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_map.find(id);
        if (it == m_map.end())
            return std::nullopt;

        T value = std::move(it->second);
        m_map.erase(it);

        return value;
    }

    bool Contains(AssetRequestID id) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_map.find(id) != m_map.end();
    }

    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_map.size();
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map.clear();
    }

private:
    std::unordered_map<AssetRequestID, T> m_map;
    mutable std::mutex m_mutex;

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
        std::lock_guard<std::mutex> lock(m_resultMutex);
        m_results.emplace(id, std::move(result));
    }

    std::optional<TResult> TakeResult(AssetRequestID id)
    {
        std::lock_guard<std::mutex> lock(m_resultMutex);

        auto it = m_results.find(id);
        if (it == m_results.end())
            return std::nullopt;

        TResult out = std::move(it->second);
        m_results.erase(it);
        return out;
    }

    bool HasResult(AssetRequestID id)
    {
        std::lock_guard<std::mutex> lock(m_resultMutex);
        return m_results.find(id) != m_results.end();
    }

private:
    AssetQueue<TRequest> m_requests;

    std::queue<AssetRequestID> m_pending;
    std::mutex m_pendingMutex;

    std::unordered_map<AssetRequestID, TResult> m_results;
    std::mutex m_resultMutex;
};