#pragma once
#include <memory>
#include "Core/Foundation/ResourceID.h"

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
AssetRequest MakeAssetRequest(Core::ResourceID resID)
{
    return
    {
        .resID = std::move(resID),
        .type = Core::GetTypeID<T>()
    };
}

template<typename T>
AssetRequest MakeAssetRequest(const std::filesystem::path& path)
{
    return MakeAssetRequest<T>(Core::ResourceID::MakePath(path));
}