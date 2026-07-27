#pragma once
#include "AssetData.h"
#include "Core/Foundation/Geometry2D.h"
#include "TextureTypes.h"

enum class PixelFormat
{
	RGB8,
	RGBA8,
};

struct TextureAsset : public AssetData
{
	CORE_DECLARE_TYPE(TextureAsset)

	Size size;
	uint32_t stride{ 0 };
	PixelFormat format{ PixelFormat::RGBA8 };
	std::vector<uint8_t> pixels{};

	ColorSpace colorSpace{ ColorSpace::SRGB };
	bool generateMipmaps{ false };
	bool isPremultipliedAlpha{ false };
};
