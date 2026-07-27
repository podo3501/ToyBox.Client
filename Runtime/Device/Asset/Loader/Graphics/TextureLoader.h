#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/TextureAsset.h"

struct IAssetMetaRegistry;

class TextureLoader : public IAssetLoader
{
public:
	explicit TextureLoader(IAssetMetaRegistry* metaRegistry) noexcept;
	virtual std::shared_ptr<AssetData> Load(AssetInput& source) override
	{
		if (source.IsStream()) return nullptr;

		auto& mem = static_cast<MemoryInput&>(source);
		return LoadFromMemory(mem.resID, std::move(mem.buffer));
	}

private:
	IAssetMetaRegistry* m_metaRegistry{ nullptr };
	std::shared_ptr<TextureAsset> LoadFromMemory(const Core::ResourceID& resID, Core::ByteBuffer buffer);
};

