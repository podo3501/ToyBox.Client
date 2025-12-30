#pragma once
#include "MouseEventType.h"

class SoundTheme
{
public:
	~SoundTheme();
	SoundTheme();
	string GetSoundID(UISoundSlotID slotID) const noexcept;
	void SetSoundID(UISoundSlotID slotID, const string& soundID) noexcept;

private:
	unordered_map<UISoundSlotID, string> m_slotToSound;
};
