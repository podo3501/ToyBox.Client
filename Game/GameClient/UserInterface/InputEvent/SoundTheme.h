#pragma once
#include "MouseEventType.h"
#include "Core/Foundation/NoCopyNoMove.h"

class SoundTheme : private NoCopyNoMove
{
public:
	~SoundTheme();
	SoundTheme();
	string GetSoundID(UISoundSlotID slotID) const noexcept;
	void SetSoundID(UISoundSlotID slotID, const string& soundID) noexcept;

private:
	unordered_map<UISoundSlotID, string> m_slotToSound;
};
