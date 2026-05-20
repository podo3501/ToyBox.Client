#pragma once

struct ITextureResource
{
	virtual ~ITextureResource() = default;
	virtual bool IsReady() const noexcept = 0;
};