#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/MeshAsset.h"

class MeshJsonLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<AssetData> Load(AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		auto& mem = static_cast<const MemoryInput&>(source);
		return LoadFromMemory(std::move(mem.buffer));
	}

private:
	std::shared_ptr<MeshAsset> LoadFromMemory(Core::ByteBuffer buffer);
};