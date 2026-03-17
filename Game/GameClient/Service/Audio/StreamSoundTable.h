#pragma once
#include "StreamSoundInfo.h"

class Serializer;
class StreamSoundTable
{
public:
	StreamSoundTable() = default;

	StreamSoundTable(const StreamSoundTable&) = delete;
	StreamSoundTable& operator=(const StreamSoundTable&) = delete;

	StreamSoundTable(StreamSoundTable&&) noexcept = default;
	StreamSoundTable& operator=(StreamSoundTable&&) noexcept = default;

	const StreamSoundInfo* GetInfo(string_view key) const noexcept;
	void Serialize(Serializer& serializer);

private:
	unordered_map<string, StreamSoundInfo> m_infos;
};