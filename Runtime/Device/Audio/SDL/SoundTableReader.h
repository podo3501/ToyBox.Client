#pragma once

struct IJsonStorage;
struct SoundInfo;
class Serializer;
class SoundTableReader
{
public:
	~SoundTableReader();
	SoundTableReader() = delete;
	explicit SoundTableReader(unique_ptr<IJsonStorage> storage);
	bool Read();
	SoundInfo* GetInfo(const string& index) noexcept;
	void Serialize(Serializer& serializer);

private:
	unique_ptr<IJsonStorage> m_storage;
	unordered_map<string, SoundInfo> m_infos;
};