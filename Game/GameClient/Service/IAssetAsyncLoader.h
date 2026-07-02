#pragma once
#include "AssetAsyncTypes.h"

struct IAssetAsyncLoader
{
	virtual ~IAssetAsyncLoader() = default;

	virtual AssetRequestID PushRequest(AssetRequest req) = 0;
	virtual AssetPtr TakeResult(AssetRequestID id) = 0;
	virtual AssetPtr Wait(AssetRequestID id) = 0;
};
