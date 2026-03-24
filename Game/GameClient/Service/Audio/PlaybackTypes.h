#pragma once

struct PlaybackParams
{
	float volume{ 1.f };
	bool loop{ false };
};

enum class PlaybackState : int
{
	Playing,
	Paused,
	Stopped,
	Count
};