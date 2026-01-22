#pragma once
#include "nlohmann/json.hpp"

class JsonFileIO
{
public:
	bool Write(const wstring& filename, const nlohmann::json& data);
	bool Read(const wstring& filename, nlohmann::json& outData);
};