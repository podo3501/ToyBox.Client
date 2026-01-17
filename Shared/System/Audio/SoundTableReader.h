#pragma once
#include "Shared/System/Public/AudioTypes.h"

struct IJsonStorage;
class Serializer;
enum class AudioGroupID;

struct SoundInfo
{
	void Serialize(Serializer& serializer);

	string filename{};
	AudioGroupID groupID{ AudioGroupID::None };
	float volume{ 0.f };
};

class SoundTableReader
{
public:
	~SoundTableReader();
	SoundTableReader() = delete;
	explicit SoundTableReader(unique_ptr<IJsonStorage> storage);
	bool Read(const wstring& filename);
	SoundInfo* GetInfo(const string& index) noexcept;
	void Serialize(Serializer& serializer);

private:
	unique_ptr<IJsonStorage> m_storage;
	unordered_map<string, SoundInfo> m_infos;
};