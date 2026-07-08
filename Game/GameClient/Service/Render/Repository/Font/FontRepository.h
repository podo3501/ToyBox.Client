#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Foundation/ResourceID.h"
#include "Service/Render/Handle/FontHandle.h"
#include "../ResourceTypes.h"

struct IFontProvider;
struct IFontResource;
struct IAssetAsyncLoader;
struct PendingFontRequest;

struct FontEntry
{
	Core::ResourceID resID;
	std::shared_ptr<IFontResource> fontRes;
	LoadState state{ LoadState::Pending };
};

class FontRepository
{
public:
	~FontRepository();
	FontRepository() = delete;
	FontRepository(IFontProvider* fontProvider, IAssetAsyncLoader* asyncLoader);
	FontHandle GetOrCreate(const Core::ResourceID& resID);
	bool Release(FontHandle h);
	void ReleaseAll();
	void Update();

	const FontEntry* Get(FontHandle h) const noexcept { return m_loadedFonts.Find(h); }

private:
	void ProcessPendingRequests();

	IFontProvider* m_fontProvider{ nullptr };
	IAssetAsyncLoader* m_asyncLoader{ nullptr };

	std::unordered_map<Core::ResourceID, FontHandle> m_cache;
	HandlePool<FontEntry, FontTag> m_loadedFonts;

	std::vector<PendingFontRequest> m_pending;
};
