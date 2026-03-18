#pragma once
#include "SoundAssetView.h"

struct IResourceManager;
class AssetRegistry
{
public:
	~AssetRegistry();
	AssetRegistry();
	bool Initialize(IResourceManager* resManager);
	const SoundAssetView* GetSoundAssetView() const noexcept;

private:
	StaticSoundDescriptors m_staticDescriptors;
	StreamSoundDescriptors m_streamDescriptors;
	SoundAssetView m_view;
};

