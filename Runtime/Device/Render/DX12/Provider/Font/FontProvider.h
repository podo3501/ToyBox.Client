#pragma once
#include "GameClient/Service/Render/Repository/Font/IFontProvider.h"

class FontProvider : public IFontProvider
{
public:
    ~FontProvider();
    FontProvider() noexcept;
    virtual shared_ptr<IFontResource> CreateResource() override;

private:
};
