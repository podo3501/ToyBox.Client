#include "pch.h"
#include "TextureMetaLoader.h"
#include "Platform/Serializer/Serializer.h"

struct JsonTextureMeta
{
	bool premultiplyAlpha{ false };

	void Serialize(Serializer& serializer)
	{
		serializer.Process("position", premultiplyAlpha);
	}
};

std::shared_ptr<TextureMetaAsset> TextureMetaLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
	nlohmann::json rData = nlohmann::json::parse(buffer.begin(), buffer.end());

	JsonTextureMeta jsonMeta;
	DeserializeClass(rData, jsonMeta);

	auto texMeta = std::make_shared<TextureMetaAsset>();
	texMeta->premultiplyAlpha = jsonMeta.premultiplyAlpha;

	return texMeta;
}

std::unique_ptr<IAssetLoader> CreateTextureMetaLoader()
{
	return make_unique<TextureMetaLoader>();
}