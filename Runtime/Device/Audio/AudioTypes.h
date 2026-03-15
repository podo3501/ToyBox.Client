#pragma once

enum class SoundType : int
{
	Static,
	Stream,
	Count
};

enum class AudioGroupID : int
{
	BGM,
	SFX,
	UI,
	System,
	Count
};

enum class PlaybackState : int
{
	Playing,
	Paused,
	Stopped,
	Count
};