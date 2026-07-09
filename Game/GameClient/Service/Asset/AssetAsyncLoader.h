#pragma once
#include "../IAssetAsyncLoader.h"
#include "AssetAsyncStore.h"
#include "Core/Foundation/ResourceID.h"
#include <mutex>

class AssetAsyncLoader : public IAssetAsyncLoader
{
public:
    virtual AssetRequestID PushRequest(AssetRequest req) override;
    virtual AssetPtr TakeResult(AssetRequestID id) override;
    virtual AssetPtr Wait(AssetRequestID id) override;

    void Shutdown();
    bool WaitPopPending(AssetRequestID& outId);
    std::optional<AssetRequest> TakeRequest(AssetRequestID id);
    void PushResult(AssetRequestID id, AssetPtr result);
    bool HasPendingWork() const;

private:
    std::atomic<bool> m_shutdown{ false };

    // Pending 그룹 (pendingMutex로 일괄 보호)
    mutable std::mutex m_pendingMutex;
    AssetAsyncStore<AssetRequest> m_requests;
    std::queue<AssetRequestID> m_pending;
    std::condition_variable m_pendingCV;

    // Wait/Result 그룹 (waitMutex로 일괄 보호)
    mutable std::mutex m_waitMutex;
    AssetAsyncStore<AssetPtr> m_results;
    std::condition_variable m_waitCV;
};