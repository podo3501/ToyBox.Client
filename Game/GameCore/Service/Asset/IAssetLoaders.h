#pragma once
#include "AssetTypes.h"
#include <memory>
#include "Core/Foundation/Types.h"

struct IAssetLoader
{
public:
	virtual ~IAssetLoader() = default;

	virtual std::shared_ptr<Asset> Load(const Core::ByteBuffer& buffer) = 0;
};

struct ITextureLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<TextureAsset> LoadTexture(const Core::ByteBuffer& buffer) = 0;

	virtual std::shared_ptr<Asset> Load(const Core::ByteBuffer& buffer) override
	{
		return LoadTexture(buffer);
	}
};
