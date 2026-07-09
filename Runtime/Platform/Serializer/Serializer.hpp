#pragma once
#include "ClassSerialize.hpp"
#include  "Concepts.h"

//JsonOperation을 사용해야 하는 타입은 여기서 분기해 준다. 그렇지 않으면 JsonTraits에서 분기
template<typename T>
void Serializer::Process(const string& key, T& data) noexcept
{
	auto writeFunc = [this, &data](nlohmann::json& j) {
		if constexpr (HasSerialize<T>) SerializeClass(j, data); //SerializeIO 가 있는 클래스. unique_ptr도 포함해서.
		else if constexpr (SequenceLike<T>) SerializeSeqContainer(j, data); //시퀀스 컨테이너일때 처리
		else if constexpr (MapLike<T>) SerializeMapContainer(j, data); //Map 컨테이너일때 처리
		else j = JsonTraits<T>::SerializeToJson(data); }; //일반적인 데이터 형식일때 처리

	auto readFunc = [this, &data](const nlohmann::json& j) {
		if constexpr (HasSerialize<T>) DeserializeClass(j, data);
		else if constexpr (SequenceLike<T>) DeserializeSeqContainer(j, data);
		else if constexpr (MapLike<T>) DeserializeMapContainer(j, data);
		else data = JsonTraits<T>::DeserializeFromJson(j); };

	ProcessImpl(key, writeFunc, readFunc);
}

template <typename ProcessFunc>
static void ProcessWriteKey(const string& key, ProcessFunc processFunc, nlohmann::json& outData) noexcept
{
	nlohmann::json writeJ{};
	processFunc(writeJ);
	outData[key] = move(writeJ);
}

template <typename ProcessFunc>
static void ProcessReadKey(const string& key, ProcessFunc processFunc, const nlohmann::json& data) noexcept
{
	if (!data.contains(key)) return;
	processFunc(data[key]);
}

template <typename WriteFunc, typename ReadFunc>
void Serializer::ProcessImpl(const string& key, WriteFunc&& writeFunc, ReadFunc&& readFunc) noexcept
{
	if (IsWrite()) 
		ProcessWriteKey(key, writeFunc, *m_write);
	else 
		ProcessReadKey(key, readFunc, *m_read);
}