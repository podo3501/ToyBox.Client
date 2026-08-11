#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "Core/FreeTypeLibrary.h"

class FontProvider : public IResourceProvider
{
public:
    ~FontProvider();
    FontProvider() noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;

private:
    FreeTypeLibrary m_ftLibrary;
    Core::ByteBuffer m_fontSource;
};
