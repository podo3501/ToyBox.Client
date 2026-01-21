#pragma once
#include "../IJsonStorage.h"

class JsonFileIO;
class JsonStorage : public IJsonStorage
{
public:
	JsonStorage() = delete;
	JsonStorage(unique_ptr<JsonFileIO> fileIO, const JsonStorageDesc& desc) noexcept;
	virtual bool Write(const string& key, const nlohmann::json& data) noexcept override;
	virtual bool Read(const string& key, nlohmann::json& outData) const noexcept override;
	virtual JsonStorageDesc* GetDescription() noexcept override;

private:
	unique_ptr<JsonFileIO> m_fileIO; //추후에 다른 IO 가 생긴다면 상속받아서 그 클래스를 생성하는 것으로 바꾸면 된다.
	JsonStorageDesc m_desc;
	mutable unordered_map<string, nlohmann::json> m_cache;
};

