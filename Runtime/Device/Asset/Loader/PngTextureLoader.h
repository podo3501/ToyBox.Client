#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"

class PngTextureLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<Asset> Load(const Core::ByteBuffer& buffer) override;
};

