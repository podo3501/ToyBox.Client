#pragma once
#include "GameClient/Service/Audio/ISoundBuffer.h"

struct IResourceStream;
class StreamSoundBuffer : public IStreamSoundBuffer
{
public:
	StreamSoundBuffer();
	virtual bool LoadFromAsset(shared_ptr<StreamSoundAsset> asset) override;
	IResourceStream* GetStream() noexcept;

private:
	unique_ptr<IResourceStream> m_fileStream;
};
