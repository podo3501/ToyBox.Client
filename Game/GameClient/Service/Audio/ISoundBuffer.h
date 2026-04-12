#pragma once

struct ISoundBuffer
{
	virtual ~ISoundBuffer() = default;
};

struct StaticSoundAsset;
struct IStaticSoundBuffer : public ISoundBuffer
{
    virtual bool LoadFromMemory(Core::ByteBuffer fileBuffer) = 0;
    virtual bool LoadFromAsset(shared_ptr<StaticSoundAsset> asset) = 0;
};

struct IResourceStream;
struct StreamSoundAsset;
struct IStreamSoundBuffer : public ISoundBuffer
{
    virtual bool AttachStream(unique_ptr<IResourceStream> stream) = 0;
    virtual bool LoadFromAsset(shared_ptr<StreamSoundAsset> asset) = 0;
};
