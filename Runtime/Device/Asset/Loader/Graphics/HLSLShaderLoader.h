#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"
#include "GameClient/Service/Asset/Assets/ShaderAsset.h"

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