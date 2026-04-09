#pragma once
#include "GameCore/Service/Asset/Asset.h"
#include "AudioTypes.h"

enum class PixelFormat
{
	RGB8,
	RGBA8,
};

struct TextureAsset : public Asset
{
	uint32_t width{ 0 };
	uint32_t height{ 0 };
	uint32_t stride{ 0 };
	PixelFormat format{ PixelFormat::RGBA8 };
	std::vector<uint8_t> pixels{};
};

class Serializer;
struct SoundDesc
{
	virtual ~SoundDesc();
	SoundDesc();
	explicit SoundDesc(SoundType _sndType);
	void Serialize(Serializer& serializer);

	SoundType sndType;
	filesystem::path filename{};
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
	const StaticSoundDesc* GetDescriptor(string_view index) const noexcept;
	void Serialize(Serializer& serializer);

private:
	unordered_map<string, StaticSoundDesc> m_descs;
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
	const StreamSoundDesc* GetDescriptor(string_view index) const noexcept;
	void Serialize(Serializer& serializer);

private:
	unordered_map<string, StreamSoundDesc> m_descs;
};

struct SoundAssetView
{
	std::shared_ptr<StaticSoundTable> staticSoundTable;
	std::shared_ptr<StreamSoundTable> streamSoundTable;
};