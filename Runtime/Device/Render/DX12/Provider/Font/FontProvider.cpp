#include "pch.h"
#include "FontProvider.h"
#include "Resource/Font/FontResource.h"

FontProvider::~FontProvider() = default;
FontProvider::FontProvider() noexcept 
{}

shared_ptr<IFontResource> FontProvider::CreateResource()
{
	return make_shared<FontResource>();
}

bool FontProvider::LoadResource(std::shared_ptr<IFontResource> resource, std::shared_ptr<FontAsset> asset)
{
	if (!resource || !asset || asset->fontSource.empty())
		return false;

	auto fontResource = std::static_pointer_cast<FontResource>(resource);

	FT_Face face{ nullptr };
	FT_Error error = FT_New_Memory_Face(
		m_ftLibrary.Get(),
		reinterpret_cast<const FT_Byte*>(asset->fontSource.data()),
		static_cast<FT_Long>(asset->fontSource.size()),
		0, // 단일 페이스 인덱스
		&face
	);

	if (error)
		return false;

	fontResource->SetFace(face);
	fontResource->MarkReady();

	return true;
}

void FontProvider::ReleaseResource(std::shared_ptr<IFontResource> resource)
{
	//resource는 여기를 벗어나면 삭제된다.
	return;
}
