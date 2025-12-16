#include "pch.h"
#include "Traits.h"
#include "../UIComponent/UIType.h"
#include "Shared/SerializerIO/Traits/TraitsHelper.hpp"

nlohmann::ordered_json JsonTraitsBase<Origin>::SerializeToJson(const Origin& data) { return EnumToString(data); }
Origin JsonTraitsBase<Origin>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<Origin>([&dataJ](Origin& data) {
		data = *StringToEnum<Origin>(dataJ); });
}
