#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "../PendingLoadQueue.h"
#include "../PendingReleaseQueue.h"

struct TextureAsset;
class TextureProvider;

class MaterialProvider : public IResourceProvider
{
public:
	~MaterialProvider();
	MaterialProvider(TaskScheduler& taskScheduler, TextureProvider& texProvider) noexcept;
	virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
	virtual void ReleaseResource(std::shared_ptr<IResource> res) override;
	void Update();

private:
	shared_ptr<IResource> CreatePbrMaterialResource(std::shared_ptr<AssetData> asset);
	shared_ptr<IResource> CreatePhongMaterialResource(std::shared_ptr<AssetData> asset);
	std::shared_ptr<TextureResource> CreateTexResource(std::shared_ptr<TextureAsset> texAsset);

	PendingLoadQueue m_pendingLoad;
	PendingReleaseQueue m_pendingRelease;
	TextureProvider& m_texProvider;
};