#pragma once
#include <memory>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

struct IJsonStorage
{
public:
	virtual ~IJsonStorage() = default;
	virtual bool Write(const std::wstring& filename, const nlohmann::json& data) = 0;
	virtual bool Read(const std::wstring& filename, nlohmann::json& outData) = 0;
};

unique_ptr<IJsonStorage> CreateJsonStorage();