#pragma once
#include <memory>

struct IAssetLoader;

struct IAssetBackend
{
	virtual ~IAssetBackend() = default;
	virtual shared_ptr<IAssetLoader> GetLoaderForExtension(string_view ext) = 0;
};

std::unique_ptr<IAssetBackend> CreateAssetBackend();