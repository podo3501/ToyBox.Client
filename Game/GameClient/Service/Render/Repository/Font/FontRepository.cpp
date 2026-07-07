#include "pch.h"
#include "FontRepository.h"
#include "IFontProvider.h"
#include "Service/IAssetAsyncLoader.h"
#include "Service/AssetAsyncHelper.h"

struct CpuPendingFontRequest
{
	FontHandle handle;
	AssetRequestID requestId;
};

FontRepository::~FontRepository() = default;
FontRepository::FontRepository(IFontProvider* fontProvider, IAssetAsyncLoader* asyncLoader) :
	m_fontProvider{ fontProvider },
	m_asyncLoader{ asyncLoader }
{}

FontHandle FontRepository::GetOrCreate(const Core::ResourceID& resID)
{
	auto it = m_cache.find(resID);
	if (it != m_cache.end())
		return it->second;

	auto fontRes = m_fontProvider->CreateResource();
	if (!fontRes) return FontHandle::Invalid();

	FontEntry entry;
	entry.resID = resID;
	entry.fontRes = std::move(fontRes);
	entry.state = LoadState::Pending;

	auto handle = m_loadedFonts.Emplace(move(entry));
	m_cache[resID] = handle;

	auto resType = resID.GetType();
	switch (resType)
	{
	case Core::ResourceIDType::Path:
	{
		//auto reqID = Asset::PushRequest<FontAsset>(m_asyncLoader, resID);
		//m_cpuPending.push_back({ handle, reqID });
	}
	break;
	}

	return FontHandle::Invalid();
}
