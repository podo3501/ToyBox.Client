#include "pch.h"
#include "Traits.h"
#include "TraitsHelper.hpp"
#include "Core/Utils/StringExt.h"

nlohmann::json JsonTraitsBase<string>::SerializeToJson(const string& data) { return data; }
string JsonTraitsBase<string>::DeserializeFromJson(const nlohmann::json& dataJ) { return dataJ; }

/////////////////////////////////////////////////////////////////

nlohmann::json JsonTraitsBase<wstring>::SerializeToJson(const wstring& data) { return WStringToString(data); }
wstring JsonTraitsBase<wstring>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<wstring>([&dataJ](wstring& data) {
		data = StringToWString(dataJ); });
}
