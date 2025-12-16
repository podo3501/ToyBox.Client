#pragma once

struct ITextureLoad;
struct ITextureBinder
{
	virtual ~ITextureBinder() {};

	virtual bool LoadResources(ITextureLoad*) = 0;
};