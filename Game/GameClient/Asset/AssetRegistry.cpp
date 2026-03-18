#include "pch.h"
#include "AssetRegistry.h"
#include "Device/Storage/JsonObjectIO.h"

AssetRegistry::~AssetRegistry() = default;
AssetRegistry::AssetRegistry() = default;

bool AssetRegistry::Initialize(IResourceManager* resManager)
{
	ReturnIfFalse(JsonObjectIO::Read(m_staticDescriptors, "Test/Sound/StaticSoundDescriptors.Json", resManager));
	ReturnIfFalse(JsonObjectIO::Read(m_streamDescriptors, "Test/Sound/StreamSoundDescriptors.Json", resManager));

	m_view.staticDescriptors = &m_staticDescriptors;
	m_view.streamDescriptors = &m_streamDescriptors;

	return true;
}

const SoundAssetView* AssetRegistry::GetSoundAssetView() const noexcept
{
	return &m_view;
}

