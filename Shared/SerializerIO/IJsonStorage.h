#pragma once
#include <memory>
#include "nlohmann/json.hpp"

struct IJsonStorage
{
public:
	virtual ~IJsonStorage() = default;
	virtual unique_ptr<IJsonStorage> Clone() const = 0;
	virtual bool Write(const std::wstring& filename, const nlohmann::ordered_json& data) = 0;
	virtual bool Read(const std::wstring& filename, nlohmann::json& outData) = 0;
};

enum class StorageType { File, Memory };
unique_ptr<IJsonStorage> CreateJsonStorage(StorageType storageType);