#pragma once
#include "GameCore/Service/Asset/IAssetBackend.h"

class STBAssetBackend : public IAssetBackend
{
public:
	~STBAssetBackend();
	STBAssetBackend();
	virtual shared_ptr<IAssetLoader> GetLoaderForExtension(string_view ext) override;

private:

};


