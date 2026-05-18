#pragma once
#include <string>
#include <typeindex>
#include <filesystem>
#include "Core/Utils/Hash.h"

inline size_t HashCombine(size_t h1, size_t h2)
{
	return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

struct LoaderKey
{
	Core::TypeId type;
	std::string ext;

	bool operator==(const LoaderKey& other) const = default;
};

struct LoaderKeyHasher
{
	size_t operator()(const LoaderKey& k) const
	{
		return Core::HashOf(k.type, k.ext);
	}
};

struct CacheKey
{
	std::filesystem::path path;
	Core::TypeId type;

	bool operator==(const CacheKey& other) const = default;
};

struct CacheKeyHasher
{
	size_t operator()(const CacheKey& k) const
	{
		return Core::HashOf(k.path, k.type);
	}
};