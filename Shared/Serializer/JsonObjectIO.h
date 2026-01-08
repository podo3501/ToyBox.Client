#pragma once
#include <string>
#include "SerializerIO.h"
#include "Shared/Serializer/Storage/IJsonStorage.h"

struct JsonObjectIO
{
public:
	template<HasProcessIO T>
	static bool Save(T& obj, IJsonStorage* storage, const std::wstring& filename);
	template<HasProcessIO T>
	static bool Load(T& obj, IJsonStorage* storage, const std::wstring& filename);
};

template<HasProcessIO T>
bool JsonObjectIO::Save(T& obj, IJsonStorage* storage, const std::wstring& filename)
{
	nlohmann::ordered_json wData;
	SerializeClassIO(obj, wData);
	return storage->Write(filename, wData);
}

template<HasProcessIO T>
bool JsonObjectIO::Load(T& obj, IJsonStorage* storage, const std::wstring& filename)
{
	nlohmann::json rData;
	ReturnIfFalse(storage->Read(filename, rData));
	DeserializeClassIO(rData, obj);

	return true;
}
