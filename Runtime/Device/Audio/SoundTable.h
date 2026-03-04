#pragma once
#include "SoundInfo.h"

class Serializer;
class SoundTable
{
public:
	SoundTable() = default;

	SoundTable(const SoundTable&) = delete;
	SoundTable& operator=(const SoundTable&) = delete;

	SoundTable(SoundTable&&) noexcept = default;
	SoundTable& operator=(SoundTable&&) noexcept = default;

	const SoundInfo* GetInfo(string_view index) const noexcept;
	void Serialize(Serializer& serializer);

private:
	unordered_map<string, SoundInfo> m_infos;
};