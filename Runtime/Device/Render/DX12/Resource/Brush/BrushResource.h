#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "../IPendingResource.h"
#include "Core/Math/Vector4.h"
#include "Core/Foundation/Geometry2D.h"

class TextureResource;

class BrushResource : public IResource, public IPendingResource
{
public:
	virtual ~BrushResource() override;
	BrushResource();
	virtual bool IsReady() const noexcept override { return m_ready; }
	virtual bool IsDependencyReady() const noexcept override;
	virtual void MarkReady() override { m_ready = true; }

	Core::Vector4 CalcUVTransform(const std::optional<Rect>& source) const;

	void SetTexture(std::shared_ptr<TextureResource> res) { m_texture = std::move(res); }	
	std::shared_ptr<TextureResource> GetTexture() { return m_texture; }
	UINT GetTextureIndex() const noexcept;

private:
	std::shared_ptr<TextureResource> m_texture;
	Core::Vector4 m_uvTransform{ 0.0f, 0.0f, 1.0f, 1.0f };

	bool m_ready{ false };
};