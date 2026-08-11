#include "pch.h"
#include "FontProvider.h"
#include "Resource/Font/FontResource.h"

FontProvider::~FontProvider() = default;
FontProvider::FontProvider() noexcept 
{}

std::shared_ptr<IResource> FontProvider::CreateResource(std::shared_ptr<AssetData> asset)
{
	if (!asset)
		return nullptr;

	auto binAsset = std::static_pointer_cast<BinaryAsset>(asset);
	if (binAsset->buffer.empty())
		return nullptr;

	auto fontRes = std::make_shared<FontResource>();
	if (!fontRes->Initialize(m_ftLibrary, std::move(binAsset)))
		return nullptr;

	return fontRes;
}

void FontProvider::ReleaseResource(std::shared_ptr<IResource> resource)
{
	//resource는 여기를 벗어나면 삭제된다.
	return;
}
