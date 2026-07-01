#include "pch.h"
#include "AssetPipelineHelper.h"

std::vector<AssetRequestID> Asset::PushRequests(
    AssetPipeline& pipeline,
    std::span<const AssetRequest> requests)
{
    std::vector<AssetRequestID> ids;
    ids.reserve(requests.size());

    for (const AssetRequest& request : requests)
        ids.emplace_back(pipeline.PushRequest(request));

    return ids;
}

std::vector<AssetPtr> Asset::WaitAll(
    AssetPipeline& pipeline, 
    std::span<const AssetRequestID> ids)
{
    std::vector<AssetPtr> results;
    results.reserve(ids.size());

    for (AssetRequestID id : ids)
        results.push_back(pipeline.Wait(id));

    return results;
}