#include "pch.h"
#include "Traits.h"
#include "Audio/AudioTypeHelpers.h"
#include "Shared/Serializer/Format/TraitsHelper.hpp"

nlohmann::json JsonTraitsBase<AudioGroupID>::SerializeToJson(const AudioGroupID& data) { return EnumToString(data); }
AudioGroupID JsonTraitsBase<AudioGroupID>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<AudioGroupID>([&dataJ](AudioGroupID& data) {
		data = *StringToEnum<AudioGroupID>(dataJ); });
}