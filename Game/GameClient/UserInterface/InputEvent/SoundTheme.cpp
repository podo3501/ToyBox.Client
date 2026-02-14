#include "pch.h"
#include "SoundTheme.h"

SoundTheme::~SoundTheme() = default;
SoundTheme::SoundTheme() = default;

string SoundTheme::GetSoundID(UISoundSlotID slotID) const noexcept
{
	auto it = m_slotToSound.find(slotID);
	if (it == m_slotToSound.end())
		return {};

	return it->second;
}

void SoundTheme::SetSoundID(UISoundSlotID slotID, const string& soundID) noexcept
{
	m_slotToSound[slotID] = soundID;
}