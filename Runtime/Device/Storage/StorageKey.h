#pragma once

namespace StorageKey
{
	struct Definition {}; //파일이 '정의' 형식일때. ex) ui component json
	struct Resource {}; //파일이 '리소스' 형식일때. ex) sound table
	struct Policy {}; //파일이 '정책' 형식일때. ex) sound theme
	struct Config {}; //파일이 'Config' 형식일때. ex) config file
	
	template<typename Key>
	constexpr const char* ResolveKey();

	template<> 
	constexpr const char* ResolveKey<Definition>() { return "Definition"; }
	template<>
	constexpr const char* ResolveKey<Resource>() { return "Resource"; }
	template<>
	constexpr const char* ResolveKey<Policy>() { return "Policy"; }
	template<>
	constexpr const char* ResolveKey<Config>() { return "Config"; }
}

template <typename Key>
concept StorageKeyType = requires
{
	{ StorageKey::ResolveKey<Key>() } -> std::convertible_to<const char*>;
};
