#include "pch.h"
#include "SoundTraits.h"
#include "AudioTypeHelpers.h"
#include "Shared/SerializerIO/Traits/TraitsHelper.hpp"

nlohmann::ordered_json JsonTraitsBase<AudioGroupID>::SerializeToJson(const AudioGroupID& data) { return EnumToString(data); }
AudioGroupID JsonTraitsBase<AudioGroupID>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<AudioGroupID>([&dataJ](AudioGroupID& data) {
		data = *StringToEnum<AudioGroupID>(dataJ); });
}