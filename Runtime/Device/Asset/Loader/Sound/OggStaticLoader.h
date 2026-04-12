#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"

class OggStaticLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<Asset> Load(const AssetInput& source) override
	{
		if (auto mem = As<MemoryInput>(source))
			return LoadFromMemory(mem->buffer);

		return nullptr;
	}

private:
	std::shared_ptr<Asset> LoadFromMemory(const Core::ByteBuffer& buffer);
};