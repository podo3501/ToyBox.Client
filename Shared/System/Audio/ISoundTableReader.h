#pragma once
#include "Shared/System/Public/AudioTypes.h"
#include <string>
#include <memory>

struct SoundInfo
{
	string filename{};
	AudioGroupID groupID{ AudioGroupID::None };
	float volume{ 0.f };
};

class SerializerIO;
struct ISoundTableReader
{
	virtual ~ISoundTableReader() = default;
	virtual bool Read(const std::wstring& filename) = 0;
	virtual SoundInfo* GetInfo(const string& index) noexcept = 0;
	virtual void ProcessIO(SerializerIO& serializer) = 0;
};

std::unique_ptr<ISoundTableReader> CreateSoundTableReader();