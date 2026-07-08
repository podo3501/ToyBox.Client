#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/ShaderAsset.h"

class HLSLShaderLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<AssetData> Load(AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		auto& mem = static_cast<MemoryInput&>(source);
		return LoadFromMemory(std::move(mem.buffer));
	}

private:
	std::shared_ptr<ShaderAsset> LoadFromMemory(Core::ByteBuffer buffer);
};