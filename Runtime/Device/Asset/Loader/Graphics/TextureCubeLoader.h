#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/TextureCubeAsset.h"

class TextureCubeLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<AssetData> Load(AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		auto& mem = static_cast<MemoryInput&>(source);
		return LoadFromMemory(std::move(mem.buffer));
	}

private:
	std::shared_ptr<TextureCubeAsset> LoadFromMemory(Core::ByteBuffer buffer);
};
