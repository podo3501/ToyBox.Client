#pragma once

struct ISoundBuffer
{
	virtual ~ISoundBuffer() = default;
};

struct StaticSoundAsset;
struct IStaticSoundBuffer : public ISoundBuffer
{
    virtual bool LoadFromAsset(shared_ptr<StaticSoundAsset> asset) = 0;
};

struct StreamSoundAsset;
struct IStreamSoundBuffer : public ISoundBuffer
{
    virtual bool LoadFromAsset(shared_ptr<StreamSoundAsset> asset) = 0;
};
