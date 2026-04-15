#pragma once
#include <memory>

struct TextureAsset;
struct ITextureResource
{
	virtual ~ITextureResource() = default;
	virtual bool LoadFromAsset(std::shared_ptr<TextureAsset> asset) = 0;
	virtual bool IsReady() const noexcept = 0;
};