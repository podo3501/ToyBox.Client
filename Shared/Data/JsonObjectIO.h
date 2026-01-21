#pragma once
#include <string>
#include "Serializer/Serializer.h"
#include "Shared/Data/Storage/IJsonStorage.h"

struct JsonObjectIO
{
	template<HasSerialize T>
	static bool Write(T& obj, IJsonStorage* storage, const std::string& key);
	template <StorageKeyType Key, HasSerialize T>
	static bool Write(T& obj, IJsonStorage* storage);

	template<HasSerialize T>
	static bool Read(T& obj, IJsonStorage* storage, const std::string& key);
	template <StorageKeyType Key, HasSerialize T>
	static bool Read(T& obj, IJsonStorage* storage);
};

template<HasSerialize T>
bool JsonObjectIO::Write(T& obj, IJsonStorage* storage, const std::string& key)
{
	nlohmann::json wData;
	SerializeClass(wData, obj);
	return storage->Write(key, wData);
}

template <StorageKeyType Key, HasSerialize T>
bool JsonObjectIO::Write(T& obj, IJsonStorage* storage)
{
	return Write(obj, storage, StorageKey::ResolveKey<Key>());
}

template<HasSerialize T>
bool JsonObjectIO::Read(T& obj, IJsonStorage* storage, const std::string& key)
{
	nlohmann::json rData;
	ReturnIfFalse(storage->Read(key, rData));
	DeserializeClass(rData, obj);

	return true;
}

template <StorageKeyType Key, HasSerialize T>
bool JsonObjectIO::Read(T& obj, IJsonStorage* storage)
{
	return Read(obj, storage, StorageKey::ResolveKey<Key>());
}
