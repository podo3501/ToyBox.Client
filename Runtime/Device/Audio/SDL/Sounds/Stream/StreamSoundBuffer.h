#pragma once

struct IResourceStream;
enum class AudioGroupID;
class StreamSoundBuffer
{
public:
	~StreamSoundBuffer();
	StreamSoundBuffer();
	bool Load(unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume);
};
