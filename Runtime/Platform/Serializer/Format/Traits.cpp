#include "pch.h"
#include "Traits.h"
#include "TraitsHelper.hpp"
#include "Platform/Utils/StringExt.h"

nlohmann::json JsonTraitsBase<string>::SerializeToJson(const string& data) { return data; }
string JsonTraitsBase<string>::DeserializeFromJson(const nlohmann::json& dataJ) { return dataJ; }

/////////////////////////////////////////////////////////////////

nlohmann::json JsonTraitsBase<wstring>::SerializeToJson(const wstring& data) { return WStringToString(data); }
wstring JsonTraitsBase<wstring>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<wstring>([&dataJ](wstring& data) {
		data = StringToWString(dataJ); });
}

/////////////////////////////////////////////////////////////////
// filesystem::path는 utf-8로 저장하고 읽어들인다.
nlohmann::json JsonTraitsBase<filesystem::path>::SerializeToJson(const filesystem::path& data) { return data.string(); }
filesystem::path JsonTraitsBase<filesystem::path>::DeserializeFromJson(const nlohmann::json& dataJ) 
{ 
	return filesystem::path(dataJ.get<string>());
}
