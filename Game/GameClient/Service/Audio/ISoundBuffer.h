#pragma once

struct AssetData;
struct ISoundBuffer
{
	virtual ~ISoundBuffer() = default;
    virtual bool LoadFromAsset(shared_ptr<AssetData> asset) = 0;
};
