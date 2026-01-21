#include "pch.h"
#include "JsonStorage.h"
#include "JsonFileIO.h"

JsonStorage::JsonStorage(unique_ptr<JsonFileIO> fileIO, const JsonStorageDesc& desc) noexcept :
	m_fileIO{ move(fileIO) },
	m_desc{ desc }
{
	for (const auto& [key, filename] : desc.Entries())
	{
		nlohmann::json data;
		m_fileIO->Read(filename, data);
		m_cache.emplace(key, move(data));
	}
}

bool JsonStorage::Write(const string& key, const nlohmann::json& data) noexcept
{
	auto filename = m_desc.GetFilename(key);
	if (filename.empty()) return false;

	ReturnIfFalse(m_fileIO->Write(filename, data));
	m_cache.insert_or_assign(key, data);

	return true;
}

bool JsonStorage::Read(const string& key, nlohmann::json& outData) const noexcept
{
	auto it = m_cache.find(key);
	if (it != m_cache.end())
	{
		outData = it->second;
		return true;
	}

	auto filename = m_desc.GetFilename(key);
	if (filename.empty()) return false;
	
	ReturnIfFalse(m_fileIO->Read(filename, outData));
	m_cache.emplace(key, outData);
	
	return true;
}

JsonStorageDesc* JsonStorage::GetDescription() noexcept
{
	return &m_desc;
}

unique_ptr<IJsonStorage> CreateJsonStorage(const JsonStorageDesc& desc)
{
	return make_unique<JsonStorage>(make_unique<JsonFileIO>(), desc);
}