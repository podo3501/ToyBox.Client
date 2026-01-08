#pragma once
#include "nlohmann/json.hpp"

struct IJsonStorage
{
public:
	virtual ~IJsonStorage() = default;
	virtual unique_ptr<ostream> OpenWrite(const wstring& filename) = 0;
	virtual unique_ptr<istream> OpenRead(const wstring& filename) = 0;

	virtual unique_ptr<IJsonStorage> Clone() const = 0;
	virtual bool Write(const wstring& filename, const nlohmann::ordered_json& data) = 0;
	virtual bool Read(const wstring& filename, nlohmann::json& outData) = 0;
};

enum class StorageType { File, Memory };
unique_ptr<IJsonStorage> CreateJsonStorage(StorageType storageType);