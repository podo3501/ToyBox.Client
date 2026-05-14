#pragma once
#include "GameClient/Service/Render/Repository/IMaterialResource.h"

struct ITextureResource;

class MaterialResource : public IMaterialResource
{
public:
	~MaterialResource();
	MaterialResource() = delete;
	explicit MaterialResource(std::shared_ptr<ITextureResource> texRes);
	virtual bool IsReady() const noexcept
	{
		return m_ready;
	}

	void MarkReady()
	{
		m_ready = true;
	}

private:
	shared_ptr<ITextureResource> m_texRes;

	bool m_ready{ false };
};