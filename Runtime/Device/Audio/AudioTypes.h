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

enum class PlayState : int
{
	Playing,
	Paused,
	Count
};