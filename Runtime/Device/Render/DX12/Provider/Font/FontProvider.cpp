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
