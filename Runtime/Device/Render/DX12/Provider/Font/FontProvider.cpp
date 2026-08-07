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

bool FontProvider::LoadResource(std::shared_ptr<IFontResource> resource, std::shared_ptr<BinaryAsset> asset)
{
	if (!resource || !asset || asset->buffer.empty())
		return false;

	auto font = std::static_pointer_cast<FontResource>(resource);
	return font->Initialize(m_ftLibrary, std::move(asset));
}

void FontProvider::ReleaseResource(std::shared_ptr<IFontResource> resource)
{
	//resource는 여기를 벗어나면 삭제된다.
	return;
}
