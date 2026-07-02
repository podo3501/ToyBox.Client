#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/ShaderAsset.h"

class HLSLShaderLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<AssetData> Load(const AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		const MemoryInput* mem = static_cast<const MemoryInput*>(&source);
		return LoadFromMemory(mem->buffer);
	}

private:
	std::shared_ptr<ShaderAsset> LoadFromMemory(const Core::ByteBuffer& buffer);
};