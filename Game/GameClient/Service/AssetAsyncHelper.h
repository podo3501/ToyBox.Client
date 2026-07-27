#pragma once
#include "IAssetAsyncLoader.h"
#include "Core/Foundation/Cast.hpp"

namespace Asset
{
    template<typename T>
    AssetRequest MakeRequest(Core::ResourceID resID)
    {
        return
        {
            .resID = std::move(resID),
            .type = Core::GetTypeID<T>()
        };
    }

    template<typename T>
    AssetRequest MakeRequest(std::string_view path)
    {
        return 
        {
            .resID = Core::ResourceID::MakePath(path),
            .type = Core::GetTypeID<T>()
        };
    }

    inline AssetRequest MakeRequest(Core::TypeID typeID, const std::string& path)
    {
        return
        {
            .resID = Core::ResourceID::MakePath(path),
            .type = typeID
        };
    }

    template<typename T>
    AssetRequestID PushRequest(IAssetAsyncLoader* asyncLoader, Core::ResourceID resID)
    {
        return asyncLoader->PushRequest(MakeRequest<T>(resID));
    }

    template<typename T>
    AssetRequestID PushRequest(IAssetAsyncLoader* asyncLoader, std::string_view path)
    {
        return PushRequest<T>(asyncLoader, Core::ResourceID::MakePath(path));
    }

    template<typename T>
    std::shared_ptr<T> TakeResult(IAssetAsyncLoader* asyncLoader, AssetRequestID id)
    {
        AssetPtr ptr = asyncLoader->TakeResult(id);
        return std::static_pointer_cast<T>(ptr);
    }

    template <typename T>
    std::shared_ptr<T> Wait(IAssetAsyncLoader* asyncLoader, AssetRequestID id)
    {
        return Core::Cast<T>(asyncLoader->Wait(id));
    }

    std::vector<AssetRequestID> PushRequests(
        IAssetAsyncLoader* asyncLoader,
        std::span<const AssetRequest> requests);

    std::vector<AssetPtr> WaitAll(
        IAssetAsyncLoader* asyncLoader,
        std::span<const AssetRequestID> ids);
}

