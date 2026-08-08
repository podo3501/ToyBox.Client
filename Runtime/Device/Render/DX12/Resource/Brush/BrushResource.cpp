#include "pch.h"
#include "BrushResource.h"
#include "../Texture/TextureResource.h"

BrushResource::~BrushResource() = default;
BrushResource::BrushResource() = default;

bool BrushResource::IsTextureReady() const noexcept
{
	// 텍스처 업로드(비동기) 완료 여부. SH는 CPU 데이터 복사라 즉시 완료됨.
	return m_texture && m_texture->IsReady();
}

Core::Vector4 BrushResource::CalcUVTransform(const Rect* source) const
{
    if (!source)
        return Core::Vector4(0.0f, 0.0f, 1.0f, 1.0f);

    const auto& size = m_texture->GetSize();

    float texW = static_cast<float>(size.width);
    float texH = static_cast<float>(size.height);

    return Core::Vector4(
        source->x / texW,
        source->y / texH,
        (source->x + source->width) / texW,
        (source->y + source->height) / texH
    );
}

UINT BrushResource::GetTextureIndex() const noexcept
{
    Assert(m_texture); // default texture 라도 들고 있어야 한다.
    return m_texture->GetHeapIndex();
}