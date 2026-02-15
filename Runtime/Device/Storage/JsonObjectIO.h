#pragma once
#include <string>
#include "Platform/Serializer/Serializer.h"
#include "IJsonStorage.h"

class JsonObjectIO
{
public:
	template <StorageKeyType Key, HasSerialize T>
	static bool Write(T& obj, IJsonStorage* storage);
	template <StorageKeyType Key, HasSerialize T>
	static bool Read(T& obj, IJsonStorage* storage);

private:
	template<HasSerialize T>
	static bool Write(T& obj, IJsonStorage* storage, const std::string& key);
	template<HasSerialize T>
	static bool Read(T& obj, IJsonStorage* storage, const std::string& key);
};

template <StorageKeyType Key, HasSerialize T>
bool JsonObjectIO::Write(T& obj, IJsonStorage* storage)
{
	return Write(obj, storage, StorageKey::ResolveKey<Key>());
}

template <StorageKeyType Key, HasSerialize T>
bool JsonObjectIO::Read(T& obj, IJsonStorage* storage)
{
	return Read(obj, storage, StorageKey::ResolveKey<Key>());
}

template<HasSerialize T>
bool JsonObjectIO::Write(T& obj, IJsonStorage* storage, const std::string& key)
{
	nlohmann::json wData;
	SerializeClass(wData, obj);
	return storage->Write(key, wData);
}

template<HasSerialize T>
bool JsonObjectIO::Read(T& obj, IJsonStorage* storage, const std::string& key)
{
	nlohmann::json rData;
	ReturnIfFalse(storage->Read(key, rData));
	DeserializeClass(rData, obj);

	return true;
}