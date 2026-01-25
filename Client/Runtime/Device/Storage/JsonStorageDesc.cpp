#include "pch.h"
#include "JsonStorageDesc.h"

void JsonStorageDesc::SetFilename(const string& key, const wstring& filename)
{
	m_entries.insert_or_assign(key, filename);
}

wstring JsonStorageDesc::GetFilename(const string& key) const noexcept
{
	auto it = m_entries.find(key);
	if (it == m_entries.end()) return L"";

	return it->second;
}

const unordered_map<string, wstring>& JsonStorageDesc::Entries() const noexcept
{
	return m_entries;
}
