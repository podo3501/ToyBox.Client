#pragma once
#include "GameCore/Service/Asset/IAssetBackend.h"

class AssetBackend : public IAssetBackend
{
public:
	~AssetBackend();
	AssetBackend();
	virtual shared_ptr<IAssetLoader> GetLoaderForExtension(string_view ext) override;

private:

};


