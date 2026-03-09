#pragma once

enum class SoundLoadMode : int
{
	Preload,
	Stream,
	None
};

enum class AudioGroupID : int
{
	BGM,
	SFX,
	UI,
	System,
	None
};

enum class PlayState : int
{
	NotLoaded,
	Stopped,
	Playing,
	None
};