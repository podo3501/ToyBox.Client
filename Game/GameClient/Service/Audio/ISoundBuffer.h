#pragma once
#include "GameClient/Service/Audio/AudioTypes.h"

struct ISoundBuffer
{
	virtual ~ISoundBuffer() = default;
};

struct IStaticSoundBuffer : public ISoundBuffer
{
    virtual bool LoadFromMemory(Core::ByteBuffer fileBuffer) = 0;
};

struct IResourceStream;
struct IStreamSoundBuffer : public ISoundBuffer
{
    virtual bool AttachStream(unique_ptr<IResourceStream> stream) = 0;
};
