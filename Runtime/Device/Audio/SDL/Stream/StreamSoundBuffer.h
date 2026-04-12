#pragma once
#include "GameClient/Service/Audio/ISoundBuffer.h"

class StreamSoundBuffer : public IStreamSoundBuffer
{
public:
	StreamSoundBuffer();
	virtual bool AttachStream(unique_ptr<IResourceStream> stream) override;
	virtual bool LoadFromAsset(shared_ptr<StreamSoundAsset> asset) override;
	IResourceStream* GetStream() noexcept;

private:
	unique_ptr<IResourceStream> m_fileStream;
};
