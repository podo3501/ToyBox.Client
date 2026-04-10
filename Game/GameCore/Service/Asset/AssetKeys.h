#pragma once
#include <string>
#include <typeindex>
#include <filesystem>

inline size_t HashCombine(size_t h1, size_t h2)
{
	return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

struct LoaderKey
{
	std::type_index type;
	std::string ext;

	bool operator==(const LoaderKey& other) const = default;
};

struct LoaderKeyHasher
{
	size_t operator()(const LoaderKey& k) const
	{
		return HashCombine(
			std::hash<std::type_index>()(k.type),
			std::hash<std::string>()(k.ext)
		);
	}
};

struct CacheKey
{
	std::filesystem::path path;
	std::type_index type;

	bool operator==(const CacheKey& other) const = default;
};

struct CacheKeyHasher
{
	size_t operator()(const CacheKey& k) const
	{
		return HashCombine(
			std::hash<std::filesystem::path>()(k.path),
			std::hash<std::type_index>()(k.type)
		);
	}
};