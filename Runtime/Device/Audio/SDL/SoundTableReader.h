#pragma once

struct SoundInfo;
class Serializer;
class SoundTableReader
{
public:
	~SoundTableReader();
	SoundTableReader();
	const SoundInfo* GetInfo(const string& index) const noexcept;
	void Serialize(Serializer& serializer);

private:
	unordered_map<string, SoundInfo> m_infos;
};