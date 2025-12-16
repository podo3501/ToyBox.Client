#pragma once

enum class AudioGroupID : int
{
	Master,
	BGM,
	SFX,
	UI,
	System,
	None,
};

enum class PlayState : int
{
	NotLoaded,
	Stopped,
	Playing
};