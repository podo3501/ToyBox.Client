#pragma once
#include "AudioTypes.h"

struct ISoundBuffer
{
	virtual ~ISoundBuffer() = default;
    virtual SoundType GetType() const = 0;
};

struct IStaticSoundBuffer : public ISoundBuffer
{
    virtual bool LoadFromMemory(Core::ByteBuffer fileBuffer) = 0;
    virtual SoundType GetType() const override { return SoundType::Static; }
};

struct IResourceStream;
struct IStreamSoundBuffer : public ISoundBuffer
{
    virtual bool AttachStream(unique_ptr<IResourceStream> stream) = 0;
    virtual SoundType GetType() const override { return SoundType::Stream; }
};
