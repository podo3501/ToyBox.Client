#pragma once
#include "GameCore/Service/Asset/Asset.h"

enum class PixelFormat
{
	RGB8,
	RGBA8,
};

struct TextureAsset : public Asset
{
	uint32_t width{ 0 };
	uint32_t height{ 0 };
	uint32_t stride{ 0 };
	PixelFormat format{ PixelFormat::RGBA8 };
	std::vector<uint8_t> pixels{};
};
