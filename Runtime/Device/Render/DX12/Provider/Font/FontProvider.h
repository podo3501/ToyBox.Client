#pragma once
#include "GameClient/Service/Render/Repository/Font/IFontProvider.h"
#include "Core/FreeTypeLibrary.h"

class FontProvider : public IFontProvider
{
public:
    ~FontProvider();
    FontProvider() noexcept;
    virtual std::shared_ptr<IFontResource> CreateResource() override;
    virtual bool LoadResource(std::shared_ptr<IFontResource> resource, std::shared_ptr<FontAsset> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IFontResource> resource) override;

private:
    FreeTypeLibrary m_ftLibrary;
    Core::ByteBuffer m_fontSource;
};
