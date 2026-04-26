#pragma once
#include <memory>

struct TextureAsset;
struct TextureDesc;
struct ITextureResource
{
	virtual ~ITextureResource() = default;
	virtual bool IsReady() const noexcept = 0;
};