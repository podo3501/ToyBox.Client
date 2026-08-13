#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"

class DebugMaterialProvider : public IResourceProvider
{
public:
	~DebugMaterialProvider();
	DebugMaterialProvider() noexcept;
	virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
	virtual void ReleaseResource(std::shared_ptr<IResource> res) override;
};