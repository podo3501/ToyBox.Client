#pragma once
#include "Resource/Resource.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Asset/TextureTypes.h"

struct TextureDesc
{
	ColorSpace colorSpace{ ColorSpace::SRGB };
	bool generateMipmaps{ false };
	bool isPremultiplyAlpha{ false };
};

class TextureResource
{
public:
	~TextureResource();
	TextureResource();
	bool IsReady() const noexcept { return m_ready; }
	void MarkReady() { m_ready = true; }

	void SetPremultiplyAlpha(bool premultiplyAlpha) { m_premultiplyAlpha = premultiplyAlpha; }

	void SetDesc(const TextureDesc& desc) { m_desc = desc; }
	const TextureDesc& GetDesc() const noexcept { return m_desc; }

	const Resource& Get() const { return m_texture; }
	Resource& Get() { return m_texture; }
	void Set(Resource resource) { m_texture = std::move(resource); }

	void SetSize(const Size& size) { m_size = size; }
	const Size& GetSize() const noexcept { return m_size; }

	void SetHeapIndex(UINT index) noexcept { m_heapIndex = index; }
	UINT GetHeapIndex() const noexcept { return m_heapIndex; }

private:
	TextureDesc m_desc;
	bool m_premultiplyAlpha{ false };
	Resource m_texture;

	Size m_size;
	UINT m_heapIndex{ UINT_MAX };
	bool m_ready{ false };
};