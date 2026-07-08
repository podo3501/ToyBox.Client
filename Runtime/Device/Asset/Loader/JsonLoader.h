#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "Platform/Serializer/Serializer.h"

template<typename T>
class JsonLoader : public IAssetLoader
{
public:
	virtual std::shared_ptr<AssetData> Load(AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		auto& mem = static_cast<MemoryInput&>(source);
		return LoadFromMemory(std::move(mem.buffer));
	}

private:
	std::shared_ptr<AssetData> LoadFromMemory(Core::ByteBuffer buffer)
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
