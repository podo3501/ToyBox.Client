#pragma once
#include "StlExtTraitsConcepts.hpp"

template<typename PointerType>
inline pair<unique_ptr<PointerType>, PointerType*> GetPtrs(unique_ptr<PointerType> uniquePtr)
{
	return make_pair(move(uniquePtr), uniquePtr.get());
}

//set, unordered_set, map, unordered_map 에 insert를 할때 없으면 insert 있으면 false를 리턴하는 stl 헬퍼함수(이걸 쓰면 속도가 빠르다)
//다른 stl은 리턴값이 다르거나 multixxx 같은 경우는 중복키를 포함하기 때문에 리턴값이 true만 나오니 불필요하다.
template<typename Container, typename... Args>
concept InsertReturnsPairWithBool = requires(Container & c, Args&&... args)
{
    { c.insert(std::forward<Args>(args)...) } -> std::same_as<std::pair<typename Container::iterator, bool>>;
};

template<typename Container, typename... Args>
    requires InsertReturnsPairWithBool<Container, Args...>
bool InsertIfAbsent(Container& c, Args&&... args) noexcept
{
    return c.insert(std::forward<Args>(args)...).second;
}

template<typename T>
bool Compare(const T& lhs, const T& rhs)
{
	if constexpr (!PointerLike<T>)
		return (lhs == rhs);

	if (!lhs && !rhs) return true;
	if (!lhs || !rhs) return false;

	bool result = (*lhs == *rhs);
	return result;
}

// 시퀀스 컨테이너 비교
template <typename Container>
bool CompareSeq(const Container& lhs, const Container& rhs)
{
	using ValueType = typename Container::value_type;

	if constexpr (!PointerLike<ValueType>)
		return lhs == rhs;

	return ranges::equal(lhs, rhs, [](const ValueType& a, const ValueType& b) {
			return Compare(a, b);
		});
}

template <typename Assoc>
bool CompareAssoc(const Assoc& lhs, const Assoc& rhs)
{
	using MappedType = typename Assoc::mapped_type;

	if constexpr (!PointerLike<MappedType>)
		return lhs == rhs;

	return ranges::equal(lhs, rhs, [](const auto& a, const auto& b) {
		return a.first == b.first && Compare(a.second, b.second); });
}

template <typename MapType>
bool CompareUnorderedAssoc(const MapType& lhs, const MapType& rhs) 
{
	using MappedType = typename MapType::mapped_type;

	if constexpr (!PointerLike<MappedType>)
		return lhs == rhs;

	if (lhs.size() != rhs.size())
		return false;

	for (const auto& [key, val] : lhs)
	{
		auto it = rhs.find(key);
		if (it == rhs.end() || !Compare(val, it->second))
			return false;
	}

	return true;
}

// 연관 컨테이너를 복사할 때 traits 사용
template<typename Assoc>
Assoc CopyAssoc(const Assoc& src)
{
	using MappedType = typename Assoc::mapped_type;
	static_assert(!RawPointerLike<MappedType>, "raw pointer 타입은 CopyAssoc에서 지원되지 않습니다.");

	if constexpr (!SmartPointerLike<MappedType>)
		return src;

	Assoc dst{};
	for (const auto& [key, value] : src)
		dst.emplace(key, value ? StlExtDetail::SmartMake<MappedType>::make(*value) : nullptr);

	return dst;
}

// try_emplace 구문상 first->second 라는게 헤깔리게 만들 수 있기 때문에 helper 함수 필요
template<typename Assoc, typename Key, typename... Args>
auto& TryEmplaceAssoc(Assoc& map, const Key& key, Args&&... args)
{
	using MappedType = typename Assoc::mapped_type;
	static_assert(!RawPointerLike<MappedType>, "raw pointer 타입은 TryEmplaceAssoc에서 지원되지 않습니다.");

	if constexpr (!SmartPointerLike<MappedType>)
		return map.try_emplace(key, MappedType(std::forward<Args>(args)...)).first->second;

	return map.try_emplace(key, StlExtDetail::SmartMake<MappedType>::make(std::forward<Args>(args)...)).first->second;
}

//원소가 몇번째에 있는지 확인
template <typename Container, typename T>
optional<int> FindIndex(const Container& container, const T& target)
{
	auto it = ranges::find(container, target);
	if (it != container.end())
		return static_cast<int>(distance(container.begin(), it));
	return nullopt;
}
