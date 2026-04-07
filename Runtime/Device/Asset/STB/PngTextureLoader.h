#pragma once
#include "GameCore/Service/Asset/IAssetLoaders.h"

class PngTextureLoader : public ITextureLoader
{
public:
	virtual std::shared_ptr<TextureAsset> LoadTexture(const Core::ByteBuffer& buffer) override;
};

