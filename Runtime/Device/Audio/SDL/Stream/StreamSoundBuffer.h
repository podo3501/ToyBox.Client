#pragma once
#include "GameClient/Service/Audio/ISoundBuffer.h"

struct IResourceStream;
class StreamSoundBuffer : public ISoundBuffer
{
public:
	StreamSoundBuffer();
	virtual bool LoadFromAsset(shared_ptr<AssetData> asset) override;
	IResourceStream* GetStream() noexcept;

private:
	shared_ptr<IResourceStream> m_fileStream;
};
