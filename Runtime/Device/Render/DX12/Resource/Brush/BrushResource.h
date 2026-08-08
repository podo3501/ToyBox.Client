#pragma once
#include "GameClient/Service/Render/Resource/IBrushResource.h"
#include "Core/Math/Vector4.h"
#include "Core/Foundation/Geometry2D.h"

class TextureResource;

class BrushResource : public IBrushResource
{
public:
	virtual ~BrushResource() override;
	BrushResource();
	virtual bool IsReady() const noexcept override { return m_ready; }
	void MarkReady() { m_ready = true; }
	bool IsTextureReady() const noexcept;
	Core::Vector4 CalcUVTransform(const Rect* source) const;

	void SetTexture(std::shared_ptr<TextureResource> res) { m_texture = std::move(res); }	
	std::shared_ptr<TextureResource> GetTexture() { return m_texture; }
	UINT GetTextureIndex() const noexcept;

private:
	std::shared_ptr<TextureResource> m_texture;
	Core::Vector4 m_uvTransform{ 0.0f, 0.0f, 1.0f, 1.0f };

	bool m_ready{ false };
};