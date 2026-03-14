#include "pch.h"
#include "Traits.h"
#include "Audio/AudioTypeHelpers.h"
#include "Platform/Serializer/Format/TraitsHelper.hpp"

using namespace EnumUtil;

nlohmann::json JsonTraitsBase<SoundType>::SerializeToJson(const SoundType& data) { return EnumToString(data); }
SoundType JsonTraitsBase<SoundType>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<SoundType>([&dataJ](SoundType& data) {
		data = *StringToEnum<SoundType>(dataJ); });
}

nlohmann::json JsonTraitsBase<AudioGroupID>::SerializeToJson(const AudioGroupID& data) { return EnumToString(data); }
AudioGroupID JsonTraitsBase<AudioGroupID>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<AudioGroupID>([&dataJ](AudioGroupID& data) {
		data = *StringToEnum<AudioGroupID>(dataJ); });
}