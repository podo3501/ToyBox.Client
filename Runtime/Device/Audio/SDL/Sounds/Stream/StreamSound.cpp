#include "pch.h"
#include "StreamSound.h"
#include "StreamSoundBuffer.h"
#include "Device/Audio/AudioTypes.h"
#include "Platform/Resource/IResourceStream.h"
#include "SDL3/SDL_init.h"

StreamSound::~StreamSound()
{
	//m_effectSoundBuffers.clear();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
StreamSound::StreamSound() = default;

bool StreamSound::Initialize()
{
	return SDL_InitSubSystem(SDL_INIT_AUDIO);
}

bool StreamSound::LoadSound(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume)
{
	StreamSoundBuffer buffer;
	ReturnIfFalse(buffer.Load(move(stream), groupID, volume));

	//m_effectSoundBuffers.insert({ filename, move(buffer) });
	return true;
}
