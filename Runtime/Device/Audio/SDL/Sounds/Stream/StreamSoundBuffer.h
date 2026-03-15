#pragma once
#include "Device/Audio/ISoundBuffer.h"

class StreamSoundBuffer : public IStreamSoundBuffer
{
public:
	StreamSoundBuffer();
	virtual bool AttachStream(unique_ptr<IResourceStream> stream) override;
	IResourceStream* GetStream() noexcept;

private:
	unique_ptr<IResourceStream> m_fileStream;
};
