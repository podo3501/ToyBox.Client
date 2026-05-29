#include "pch.h"
#include "GameClient/Service/Asset/Assets/SoundTableAsset.h"
#include "Platform/Serializer/Serializer.h"
#include "Platform/Serializer/Format/TraitsHelper.hpp"
#include "Platform/Serializer/Format/Traits.h"

DECLARE_JSON_TRAITS(SoundType)
DECLARE_JSON_TRAITS(Core::ResourceID)
DECLARE_JSON_TRAITS(AudioGroup)


using namespace EnumUtil;

template<>
inline constexpr auto EnumUtil::EnumToStringMap<SoundType> = std::array{
	"Static",
	"Stream"
};
ASSERT_ENUM_COUNT(SoundType);

template<>
inline constexpr auto EnumUtil::EnumToStringMap<AudioGroup> = std::array{
	"BGM",
	"SFX",
	"UI",
	"System"
};
ASSERT_ENUM_COUNT(AudioGroup);

nlohmann::json JsonTraitsBase<SoundType>::SerializeToJson(const SoundType& data) { return EnumToString(data); }
SoundType JsonTraitsBase<SoundType>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<SoundType>([&dataJ](SoundType& data) {
		data = *StringToEnum<SoundType>(dataJ); });
}

nlohmann::json JsonTraitsBase<Core::ResourceID>::SerializeToJson(const Core::ResourceID& data) { return data.GetValue(); }
Core::ResourceID JsonTraitsBase<Core::ResourceID>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return Core::ResourceID::MakePath(dataJ.get<string>());
}

nlohmann::json JsonTraitsBase<AudioGroup>::SerializeToJson(const AudioGroup& data) { return EnumToString(data); }
AudioGroup JsonTraitsBase<AudioGroup>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<AudioGroup>([&dataJ](AudioGroup& data) {
		data = *StringToEnum<AudioGroup>(dataJ); });
}

SoundDesc::~SoundDesc() = default;
SoundDesc::SoundDesc() :
	sndType{ EnumUtil::Invalid<SoundType> },
	group{ EnumUtil::Invalid<AudioGroup> }
{
}

SoundDesc::SoundDesc(SoundType _sndType) :
	sndType{ _sndType }
{
}

void SoundDesc::Serialize(Serializer& serializer)
{
	serializer.Process("Filename", resID);
	serializer.Process("Group", group);
	serializer.Process("Priority", priority);
	serializer.Process("Volume", volume);
}
