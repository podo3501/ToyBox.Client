#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Foundation/ResourceID.h"
#include "Service/Render/Handle/FontHandle.h"
#include "../ResourceTypes.h"

struct IFontProvider;
struct IFontResource;
struct IAssetAsyncLoader;
struct CpuPendingFontRequest;

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

private:
	IFontProvider* m_fontProvider{ nullptr };
	IAssetAsyncLoader* m_asyncLoader{ nullptr };

	std::unordered_map<Core::ResourceID, FontHandle> m_cache;
	HandlePool<FontEntry, FontTag> m_loadedFonts;

	std::vector<CpuPendingFontRequest> m_cpuPending;
};
