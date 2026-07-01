#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"
#include "Platform/Serializer/Serializer.h"

template<typename T>
class JsonLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<AssetData> Load(const AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		const MemoryInput* mem = static_cast<const MemoryInput*>(&source);
		return LoadFromMemory(mem->buffer);
	}

private:
	std::shared_ptr<AssetData> LoadFromMemory(const Core::ByteBuffer& buffer)
	{
		auto asset = std::make_shared<T>();

		nlohmann::json rData = nlohmann::json::parse(buffer.begin(), buffer.end());
		DeserializeClass(rData, *asset);

		return asset;
	}
};

template<typename T>
std::unique_ptr<IAssetLoader> CreateJsonLoader()
{
	return std::make_unique<JsonLoader<T>>();
}
