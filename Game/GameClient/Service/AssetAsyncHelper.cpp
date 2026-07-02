#include "pch.h"
#include "AssetAsyncHelper.h"

std::vector<AssetRequestID> Asset::PushRequests(
    IAssetAsyncLoader* asyncLoader,
    std::span<const AssetRequest> requests)
{
    std::vector<AssetRequestID> ids;
    ids.reserve(requests.size());

    for (const AssetRequest& request : requests)
        ids.emplace_back(asyncLoader->PushRequest(request));

    return ids;
}

std::vector<AssetPtr> Asset::WaitAll(
    IAssetAsyncLoader* asyncLoader,
    std::span<const AssetRequestID> ids)
{
    std::vector<AssetPtr> results;
    results.reserve(ids.size());

    for (AssetRequestID id : ids)
        results.push_back(asyncLoader->Wait(id));

    return results;
}