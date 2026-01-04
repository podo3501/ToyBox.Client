#pragma once
#include "Shared/Utils/StlExtTraitsConcepts.hpp"
#include <concepts>
#include <type_traits>
#include <string>

// SerializeIO 지원 여부 확인
class SerializerIO;
template<typename T>
concept HasProcessIO =
	requires(T t, SerializerIO& serializer) {
		{ t->ProcessIO(serializer) };   // 스마트 포인터용
	} ||
	requires(T t, SerializerIO& serializer) {
		{ t.ProcessIO(serializer) };    // 일반 객체용
	};

template<typename T> //멤버 begin/end가 있는 경우
concept HasMemberBeginEnd = requires(T t) {
	{ t.begin() } -> std::input_iterator;
	{ t.end() } -> std::sentinel_for<decltype(t.begin())>;
};

template<typename T>
concept NotBasicString =
!(std::same_as<std::remove_cvref_t<T>, std::string> ||
	std::same_as<std::remove_cvref_t<T>, std::wstring> ||
	std::same_as<std::remove_cvref_t<T>, std::u16string> ||
	std::same_as<std::remove_cvref_t<T>, std::u32string>);

template<typename T>
concept SequenceLike =
!requires { typename T::key_type; }&&     // key_type 없어야 함 (map 방지)
	requires { typename T::value_type; }&&    // value_type 있어야 함 (sequence 조건)
HasMemberBeginEnd<T> &&
NotBasicString<T>;    // 문자열 계열 제외

template<typename T>
concept MapLike = requires {
	typename T::key_type;
	typename T::mapped_type; }&&
	HasMemberBeginEnd<T>;

//template<typename T> //이것이 unique, shared 만 찾아주는게 아니라 element_type 이 있는 것을 찾아주는 건데, 대부분 이 타입이 있으면 smartPtr만 있기 때문이다.
//concept SmartPointerLike = requires {
//	typename T::element_type; };