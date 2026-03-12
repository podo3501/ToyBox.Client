#pragma once

enum class SoundType : int
{
	Static,
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
	Playing,
	None
};