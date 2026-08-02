#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/CubemapAsset.h"

class KtxTextureLoader : public IAssetLoader
{
public:
	explicit KtxTextureLoader(IAssetMetaRegistry* metaRegistry) noexcept;
	virtual std::shared_ptr<AssetData> Load(AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		auto& mem = static_cast<MemoryInput&>(source);
		return LoadFromMemory(mem.resID, std::move(mem.buffer));
	}

private:
	std::shared_ptr<CubemapAsset> LoadFromMemory(const Core::ResourceID& resID, Core::ByteBuffer buffer);

	IAssetMetaRegistry* m_metaRegistry{ nullptr };
};