#pragma once
#include <memory>

struct SoundDesc;
struct ISoundBuffer;

enum class SoundLoadState
{
    Pending,
    Ready,
    Failed
};

struct LoadedSound
{
	const SoundDesc* desc{ nullptr }; //다형성을 위한 포인터. 이값의 원본은 프로그램이 죽을때까지 살아있다는게 보장됨.
	std::shared_ptr<ISoundBuffer> buffer;
    SoundLoadState state{ SoundLoadState::Pending };
};