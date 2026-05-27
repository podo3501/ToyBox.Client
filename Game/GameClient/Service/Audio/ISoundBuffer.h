#pragma once

struct Asset;
struct ISoundBuffer
{
	virtual ~ISoundBuffer() = default;
    virtual bool LoadFromAsset(shared_ptr<Asset> asset) = 0;
};
