#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/MeshAsset.h"

class MeshJsonLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<AssetData> Load(const AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		const MemoryInput* mem = static_cast<const MemoryInput*>(&source);
		return LoadFromMemory(mem->buffer);
	}

private:
	std::shared_ptr<MeshAsset> LoadFromMemory(const Core::ByteBuffer& buffer);
};