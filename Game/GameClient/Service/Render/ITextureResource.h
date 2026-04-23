#pragma once
#include <memory>

struct TextureAsset;
struct TextureDesc;
struct ITextureResource
{
	virtual ~ITextureResource() = default;
	//virtual bool LoadFromAsset(std::shared_ptr<TextureAsset> asset, const TextureDesc& desc) = 0;
	virtual bool IsReady() const noexcept = 0;
};