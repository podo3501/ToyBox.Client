#pragma once

#include <memory>

struct Asset;
struct AssetRequest;

template<typename TRequest, typename TResult>
class AssetPipeline;

template<typename TRequest, typename TResult>
class AssetWorker;

using AssetPtr = std::shared_ptr<Asset>;
using AssetPipelineT = AssetPipeline<AssetRequest, AssetPtr>;
using AssetWorkerT = AssetWorker<AssetRequest, AssetPtr>;

template<typename T>
AssetRequest MakeAssetRequest(std::filesystem::path path)
{
    return
    {
        .path = std::move(path),
        .type = Core::GetTypeID<T>()
    };
}