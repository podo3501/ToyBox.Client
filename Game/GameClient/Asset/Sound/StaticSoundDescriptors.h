#pragma once
#include "Desc/StaticSoundDescriptor.h"

class Serializer;
class StaticSoundDescriptors
{
public:
	StaticSoundDescriptors() = default;

	StaticSoundDescriptors(const StaticSoundDescriptors&) = delete;
	StaticSoundDescriptors& operator=(const StaticSoundDescriptors&) = delete;

	StaticSoundDescriptors(StaticSoundDescriptors&&) noexcept = default;
	StaticSoundDescriptors& operator=(StaticSoundDescriptors&&) noexcept = default;

	const StaticSoundDescriptor* GetDescriptor(string_view key) const noexcept;
	void Serialize(Serializer& serializer);

private:
	unordered_map<string, StaticSoundDescriptor> m_descs;
};