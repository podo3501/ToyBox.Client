#pragma once
#include "Desc/StreamSoundDescriptor.h"

class Serializer;
class StreamSoundDescriptors
{
public:
	StreamSoundDescriptors() = default;

	StreamSoundDescriptors(const StreamSoundDescriptors&) = delete;
	StreamSoundDescriptors& operator=(const StreamSoundDescriptors&) = delete;

	StreamSoundDescriptors(StreamSoundDescriptors&&) noexcept = default;
	StreamSoundDescriptors& operator=(StreamSoundDescriptors&&) noexcept = default;

	const StreamSoundDescriptor* GetDescriptor(string_view key) const noexcept;
	void Serialize(Serializer& serializer);

private:
	unordered_map<string, StreamSoundDescriptor> m_descs;
};