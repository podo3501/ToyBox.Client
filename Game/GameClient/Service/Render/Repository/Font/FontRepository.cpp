#include "pch.h"
#include "FontRepository.h"
#include "IFontProvider.h"
#include "Service/IAssetAsyncLoader.h"
#include "Service/AssetAsyncHelper.h"

struct PendingFontRequest
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
	entry.state = LoadState::CpuLoading;

	auto handle = m_loadedFonts.Emplace(move(entry));
	m_cache[resID] = handle;

	auto resType = resID.GetType();
	switch (resType)
	{
	case Core::ResourceIDType::Path:
	{
		auto reqID = Asset::PushRequest<BinaryAsset>(m_asyncLoader, resID);
		m_pending.push_back({ handle, reqID });
	}
	break;
	case Core::ResourceIDType::Runtime:
	case Core::ResourceIDType::Builtin:
	{
		//일단 보류
	}
	break;
	default:
		return FontHandle::Invalid();
	}

	return handle;
}

bool FontRepository::Release(FontHandle h)
{
	auto entry = m_loadedFonts.Find(h);
	if (!entry) return false;

	m_cache.erase(entry->resID);

	m_fontProvider->ReleaseResource(std::move(entry->fontRes));
	return m_loadedFonts.Remove(h);
}

void FontRepository::ReleaseAll()
{
	m_loadedFonts.Visit([this](FontHandle h, FontEntry&) {
		Release(h);
		});

	m_pending.clear();
	m_cache.clear();
	m_loadedFonts.Clear();
}

void FontRepository::Update()
{
	ProcessPendingRequests();
}

void FontRepository::ProcessPendingRequests()
{
	if (m_pending.empty()) return;

	for (auto it = m_pending.begin(); it != m_pending.end(); )
	{
		auto& req = *it;
		auto asset = m_asyncLoader->TakeResult(req.requestId);
		if (!asset)
		{
			++it;
			continue;
		}

		// CPU 비동기 파일 스트리밍이 완료된 시점
		auto entry = m_loadedFonts.Find(req.handle);
		if (entry && entry->fontRes)
		{
			auto binaryAsset = std::static_pointer_cast<BinaryAsset>(asset);
			if (m_fontProvider->LoadResource(entry->fontRes, binaryAsset))
			{
				Assert(entry->fontRes->IsReady()); //로딩했다면 준비상태여야 한다.
				entry->state = LoadState::Ready;
			}
			else
				entry->state = LoadState::Failed;
		}

		it = m_pending.erase(it);
	}
}

