#pragma once
#include "MouseEventType.h"

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
