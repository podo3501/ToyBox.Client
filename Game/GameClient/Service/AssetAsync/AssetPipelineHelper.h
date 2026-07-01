#pragma once
#include "AssetAsyncTypes.h"
#include "AssetPipeline.h"

namespace Asset
{
    std::vector<AssetRequestID> PushRequests(
        AssetPipeline& pipeline,
        std::span<const AssetRequest> requests);

    std::vector<AssetPtr> WaitAll(
        AssetPipeline& pipeline,
        std::span<const AssetRequestID> ids);

 //   template<typename TResult>
 //   std::vector<TResult> WaitAll(AssetPipelineT& pipeline, std::span<const AssetRequestID> ids)
 //   {
 //       std::vector<TResult> results;
 //       results.reserve(ids.size());

 //       for (AssetRequestID id : ids)
 //           results.push_back(pipeline.Wait(id));

 //       return results;
 //   }
}

