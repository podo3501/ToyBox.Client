#pragma once
#include "StorageKey.h"

class JsonStorageDesc
{
public:
	template<StorageKeyType Key>
	void SetFilename(const wstring& filename) { SetFilename(StorageKey::ResolveKey<Key>(), filename); }
	void SetFilename(const string& key, const wstring& filename);
	template<StorageKeyType Key>
	wstring GetFilename() const noexcept { return GetFilename(StorageKey::ResolveKey<Key>()); }
	wstring GetFilename(const string& key) const noexcept;
	const unordered_map<string, wstring>& Entries() const noexcept;

private:
	unordered_map<string, wstring> m_entries;
};