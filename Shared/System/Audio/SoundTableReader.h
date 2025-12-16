#pragma once
#include "ISoundTableReader.h"

struct SoundInfoImpl;
class SerializerIO;
enum class AudioGroupID;
class SoundTableReader : public ISoundTableReader
{
public:
	~SoundTableReader();
	SoundTableReader();
	virtual bool Read(const wstring& filename) override;
	virtual SoundInfo* GetInfo(const string& index) noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;

private:
	unordered_map<string, SoundInfoImpl> m_infos;
};


