#pragma once
#include "Format/Traits.h"
#include "Format/KeyConverter.hpp"
#include "Concepts.h"

//새로운 ProcessIO를 사용하는 클래스를 만들면 여기서 Extension을 만들어서 사용한다.
//SerializeClassIO 및 DeserializeClassIO 함수를 각 클래스에 맞게 정의해서 파일을 만든다.

template<typename T>
inline void SerializeClass_GenerateJson(T& data, nlohmann::ordered_json& outWriteJ)
{
	Serializer serializer{ outWriteJ };
	if constexpr (RawPointerLike<T>)
		data->ProcessIO(serializer);
	else
		data.ProcessIO(serializer);
}

template<typename T>
void SerializeClass_Internal(T& data, nlohmann::ordered_json& j) { SerializeClass_GenerateJson(data, j); }

template<typename T>
void SerializeClass(T& data, nlohmann::ordered_json& j) { SerializeClass_Internal(data, j); }
template<typename T>
void SerializeClass(unique_ptr<T>& data, nlohmann::ordered_json& j) { SerializeClass_Internal(*data, j); }

///////////////////////////////////////////////////////

template<typename T>
void DeserializeClass_Internal(const nlohmann::json& outReadJ, T& data)
{
	Serializer serializer{ outReadJ };
	if constexpr (RawPointerLike<T>)
		data->ProcessIO(serializer);
	else 
		data.ProcessIO(serializer);
}

template<typename T>
void DeserializeClass(const nlohmann::json& j, T& data)
{
	DeserializeClass_Internal(j, data);
}

template<typename T>
void DeserializeClass(const nlohmann::json& j, unique_ptr<T>& data)
{
	data = make_unique<T>();
	DeserializeClass_Internal(j, *data);
}

template<typename T>
nlohmann::ordered_json SerializeByType(T& v)
{
	if constexpr (HasProcessIO<T>)
	{
		nlohmann::ordered_json j{};
		SerializeClass(v, j);
		return j;
	}
	else
		return JsonTraits<T>::SerializeToJson(v);
}

template<typename T>
T DeserializeByType(const nlohmann::json& v)
{
	if constexpr (HasProcessIO<T>)
	{
		T data{};
		DeserializeClass(v, data);
		return data;
	}
	else
		return JsonTraits<T>::DeserializeFromJson(v);
}

template<typename MapContainer>
void SerializeMapContainer(MapContainer& datas, nlohmann::ordered_json& j)
{
	using T = typename MapContainer::mapped_type;

	for (auto& [k, v] : datas)
	{
		const auto key = ToKeyString(k);
		j.emplace(key, SerializeByType<T>(v));
	}
}

template<typename MapContainer>
void DeserializeMapContainer(const nlohmann::json& j, MapContainer& datas)
{
	using K = typename MapContainer::key_type;
	using T = typename MapContainer::mapped_type;

	for (const auto& [k, v] : j.items())
	{
		const auto key = FromKeyString<K>(k);
		datas.emplace(key, DeserializeByType<T>(v));
	}
}

template<typename SeqContainer>
void SerializeSeqContainer(SeqContainer& datas, nlohmann::ordered_json& j)
{
	for (auto& data : datas)
		j.push_back(SerializeByType(data));
}

template<typename SeqContainer>
void DeserializeSeqContainer(const nlohmann::json& j, SeqContainer& datas)
{
	for (const auto& dataJ : j)
	{
		using T = typename SeqContainer::value_type;
		datas.emplace_back(DeserializeByType<T>(dataJ));
	}
}