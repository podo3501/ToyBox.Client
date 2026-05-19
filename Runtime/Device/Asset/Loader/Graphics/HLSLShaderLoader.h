#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"

class HLSLShaderLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<Asset> Load(const AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		const MemoryInput* mem = static_cast<const MemoryInput*>(&source);
		return LoadFromMemory(mem->buffer);
	}

private:
	std::shared_ptr<Asset> LoadFromMemory(const Core::ByteBuffer& buffer);
};