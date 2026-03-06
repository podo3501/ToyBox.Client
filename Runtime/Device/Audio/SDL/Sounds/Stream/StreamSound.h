#pragma once

struct IResourceStream;
class StreamSoundBuffer;
enum class AudioGroupID;
class StreamSound
{
public:
	~StreamSound();
	StreamSound();
	bool Initialize();
	bool LoadSound(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume);

private:
	unordered_map<filesystem::path, StreamSoundBuffer> m_streamSoundBuffers;
};
