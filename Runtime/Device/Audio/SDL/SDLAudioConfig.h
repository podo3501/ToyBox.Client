#pragma once

struct VoiceLimits
{
	static constexpr int MaxVoices = 64;
	static constexpr int MaxStreams = 16;
};

struct StreamAudioRequestDevice //스트림용 Device를 만들때 요청하는 값(실제로는 이렇게 안 만들어질수도 있음. 코드에서 확인)
{ 
	int freq = 48000; 
	SDL_AudioFormat format = SDL_AUDIO_F32; 
	int channels = 2; 
};
