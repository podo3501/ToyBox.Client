#include "pch.h"
#include "TextureMetaLoader.h"
#include "Platform/Serializer/Serializer.h"
#include "Platform/Serializer/Format/TraitsHelper.hpp"
#include "Platform/Serializer/Format/Traits.h"

using namespace EnumUtil;

template<>
inline constexpr auto EnumUtil::EnumToStringMap<ColorSpace> = std::array{
	"SRGB",
	"Linear"
};
ASSERT_ENUM_COUNT(ColorSpace);

nlohmann::json JsonTraitsBase<ColorSpace>::SerializeToJson(const ColorSpace& data) { return EnumToString(data); }
ColorSpace JsonTraitsBase<ColorSpace>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<ColorSpace>([&dataJ](ColorSpace& data) {
		data = *StringToEnum<ColorSpace>(dataJ); });
}

template<>
inline constexpr auto EnumUtil::EnumToStringMap<AlphaSourceState> = std::array{
	"Opaque",
	"Straight",
	"AlreadyPremultiplied"
};
ASSERT_ENUM_COUNT(AlphaSourceState);

nlohmann::json JsonTraitsBase<AlphaSourceState>::SerializeToJson(const AlphaSourceState& data) { return EnumToString(data); }
AlphaSourceState JsonTraitsBase<AlphaSourceState>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<AlphaSourceState>([&dataJ](AlphaSourceState& data) {
		data = *StringToEnum<AlphaSourceState>(dataJ); });
}

template<>
inline constexpr auto EnumUtil::EnumToStringMap<BlendTargetSpace> = std::array{
	"NonPremultiplied",
	"Premultiplied",
	"None"
};
ASSERT_ENUM_COUNT(BlendTargetSpace);

nlohmann::json JsonTraitsBase<BlendTargetSpace>::SerializeToJson(const BlendTargetSpace& data) { return EnumToString(data); }
BlendTargetSpace JsonTraitsBase<BlendTargetSpace>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<BlendTargetSpace>([&dataJ](BlendTargetSpace& data) {
		data = *StringToEnum<BlendTargetSpace>(dataJ); });
}

struct JsonTextureMeta
{
	ColorSpace colorSpace{ ColorSpace::SRGB };
	bool generateMipmaps{ false };
	AlphaSourceState alphaSourceState{ AlphaSourceState::Straight };
	BlendTargetSpace blendTargetSpace{ BlendTargetSpace::NonPremultiplied };

	void Serialize(Serializer& serializer)
	{
		serializer.Process("ColorSpace", colorSpace);
		serializer.Process("GenerateMipmaps", generateMipmaps);
		serializer.Process("AlphaSourceState", alphaSourceState);
		serializer.Process("BlendTargetSpace", blendTargetSpace);
	}
};

std::shared_ptr<TextureMetaAsset> TextureMetaLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
	nlohmann::json rData = nlohmann::json::parse(buffer.begin(), buffer.end());

	JsonTextureMeta jsonMeta;
	DeserializeClass(rData, jsonMeta);

	auto texMeta = std::make_shared<TextureMetaAsset>();
	texMeta->colorSpace = jsonMeta.colorSpace;
	texMeta->generateMipmaps = jsonMeta.generateMipmaps;
	texMeta->alphaSourceState = jsonMeta.alphaSourceState;
	texMeta->blendTargetSpace = jsonMeta.blendTargetSpace;

	return texMeta;
}

std::unique_ptr<IAssetLoader> CreateTextureMetaLoader()
{
	return make_unique<TextureMetaLoader>();
}