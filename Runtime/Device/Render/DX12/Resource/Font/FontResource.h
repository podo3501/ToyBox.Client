#pragma once
#include "GameClient/Service/Render/Resource/IFontResource.h"

class FontResource : public IFontResource
{
public:
	~FontResource();
	FontResource();
	virtual bool IsReady() const noexcept override { return m_ready; }

private:
	bool m_ready{ false };
};
