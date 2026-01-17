#pragma once
#include <string>
#include "Serializer.h"
#include "Shared/SerializerIO/IJsonStorage.h"

struct JsonObjectIO
{
public:
	template<HasSerialize T>
	static bool Save(T& obj, IJsonStorage* storage, const std::wstring& filename);
	template<HasSerialize T>
	static bool Load(T& obj, IJsonStorage* storage, const std::wstring& filename);
};

template<HasSerialize T>
bool JsonObjectIO::Save(T& obj, IJsonStorage* storage, const std::wstring& filename)
{
	nlohmann::json wData;
	SerializeClass(wData, obj);
	return storage->Write(filename, wData);
}

template<HasSerialize T>
bool JsonObjectIO::Load(T& obj, IJsonStorage* storage, const std::wstring& filename)
{
	nlohmann::json rData;
	ReturnIfFalse(storage->Read(filename, rData));
	DeserializeClass(rData, obj);

	return true;
}
