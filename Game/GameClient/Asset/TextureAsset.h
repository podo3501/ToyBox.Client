#pragma once
#include "AssetData.h"

enum class PixelFormat
{
	RGB8,
	RGBA8,
};

struct TextureAsset : public AssetData
{
	CORE_DECLARE_TYPE(TextureAsset)

	uint32_t width{ 0 };
	uint32_t height{ 0 };
	uint32_t stride{ 0 };
	PixelFormat format{ PixelFormat::RGBA8 };
	std::vector<uint8_t> pixels{};
};
