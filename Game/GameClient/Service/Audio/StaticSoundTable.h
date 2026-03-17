#pragma once

#pragma once
#include "StaticSoundInfo.h"

class Serializer;
class StaticSoundTable
{
public:
	StaticSoundTable() = default;

	StaticSoundTable(const StaticSoundTable&) = delete;
	StaticSoundTable& operator=(const StaticSoundTable&) = delete;

	StaticSoundTable(StaticSoundTable&&) noexcept = default;
	StaticSoundTable& operator=(StaticSoundTable&&) noexcept = default;

	const StaticSoundInfo* GetInfo(string_view key) const noexcept;
	void Serialize(Serializer& serializer);

private:
	unordered_map<string, StaticSoundInfo> m_infos;
};