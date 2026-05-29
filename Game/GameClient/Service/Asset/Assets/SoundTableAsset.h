#pragma once
#include "Core/Foundation/ResourceID.h"
#include "GameCore/Service/Asset/Asset.h"
#include "AudioTypes.h"
#include <filesystem>
#include <unordered_map>
#include <string>

class Serializer;
struct SoundDesc
{
	virtual ~SoundDesc();
	SoundDesc();
	explicit SoundDesc(SoundType _sndType);
	void Serialize(Serializer& serializer);

	SoundType sndType;
	Core::ResourceID resID{};
	AudioGroup group;
	int priority{ 0 };
	float volume{ 0.f };
};

struct StaticSoundDesc : public SoundDesc
{
	StaticSoundDesc();
	void Serialize(Serializer& serializer);

	//static 항목이 생기면 여기에 추가
};

struct StaticSoundTable : public Asset
{
public:
	CORE_DECLARE_TYPE(StaticSoundTable)

	const StaticSoundDesc* GetDescriptor(std::string_view index) const noexcept;
	void Serialize(Serializer& serializer);

private:
	std::unordered_map<std::string, StaticSoundDesc> m_descs;
};

struct StreamSoundDesc : public SoundDesc
{
	StreamSoundDesc();
	void Serialize(Serializer& serializer);

	bool loop{ false };
};

struct StreamSoundTable : public Asset
{
public:
	CORE_DECLARE_TYPE(StreamSoundTable)

	const StreamSoundDesc* GetDescriptor(std::string_view index) const noexcept;
	void Serialize(Serializer& serializer);

private:
	std::unordered_map<std::string, StreamSoundDesc> m_descs;
};

struct SoundAssetView
{
	std::shared_ptr<StaticSoundTable> staticSoundTable;
	std::shared_ptr<StreamSoundTable> streamSoundTable;
};