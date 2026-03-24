#include "pch.h"
#include "AudioTraits.h"
#include "../../AudioTypeHelpers.h"
#include "Platform/Serializer/Format/TraitsHelper.hpp"

using namespace EnumUtil;

nlohmann::json JsonTraitsBase<SoundType>::SerializeToJson(const SoundType& data) { return EnumToString(data); }
SoundType JsonTraitsBase<SoundType>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<SoundType>([&dataJ](SoundType& data) {
		data = *StringToEnum<SoundType>(dataJ); });
}

nlohmann::json JsonTraitsBase<AudioGroup>::SerializeToJson(const AudioGroup& data) { return EnumToString(data); }
AudioGroup JsonTraitsBase<AudioGroup>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<AudioGroup>([&dataJ](AudioGroup& data) {
		data = *StringToEnum<AudioGroup>(dataJ); });
}