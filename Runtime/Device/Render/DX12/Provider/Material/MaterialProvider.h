#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "../IUpdatableProvider.h"

struct PbrSurface;
struct PhongSurface;
struct TextureAsset;
class PendingLoadQueue;
class PendingReleaseQueue;
class TextureProvider;
class TextureResource;
enum class BuiltinTextureType;

class MaterialProvider : public IResourceProvider
{
public:
	~MaterialProvider();
	MaterialProvider(
		PendingLoadQueue& pendingLoad,
		PendingReleaseQueue& pendingRelease, 
		TextureProvider& texProvider) noexcept;
	virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
	virtual void ReleaseResource(std::shared_ptr<IResource> res) override;

private:
	shared_ptr<IResource> BuildPbrResource(
		std::shared_ptr<TextureAsset> albedoAsset,
		std::shared_ptr<TextureAsset> normalAsset,
		std::shared_ptr<TextureAsset> armAsset,
		const PbrSurface& surface);

	shared_ptr<IResource> BuildPhongResource(
		std::shared_ptr<TextureAsset> albedoAsset,
		std::shared_ptr<TextureAsset> normalAsset,
		const PhongSurface& surface);

	std::shared_ptr<TextureResource> CreateTexResource(std::shared_ptr<TextureAsset> texAsset);
	std::shared_ptr<TextureResource> CreateTexResourceOrFallback(
		std::shared_ptr<TextureAsset> texAsset,
		BuiltinTextureType fallbackType);

	PendingLoadQueue& m_pendingLoad;
	PendingReleaseQueue& m_pendingRelease;
	TextureProvider& m_texProvider;
};