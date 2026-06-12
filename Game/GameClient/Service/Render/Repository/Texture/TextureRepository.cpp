#include "pch.h"
#include "TextureRepository.h"
#include "ITextureProvider.h"
#include "Service/Render/Resource/ITextureResource.h"
#include "Service/AssetAsync/AssetPipeline.h"

struct CpuPendingTextureRequest
{
	TextureHandle handle;
	AssetRequestID requestId;
};

struct GpuPendingTextureRequest
{
	TextureHandle handle;
	std::shared_ptr<TextureAsset> texAsset;
};

TextureRepository::~TextureRepository() { ReleaseAll(); }
TextureRepository::TextureRepository(ITextureProvider* texProvider, AssetPipelineT* assetPipeline) :
	m_texProvider{ texProvider },
	m_assetPipeline{ assetPipeline }
{}

TextureHandle TextureRepository::GetOrCreate(const TextureDesc& desc, std::shared_ptr<TextureAsset> asset)
{
	const auto& key = desc.GetHash();
	auto it = m_cache.find(key);
	if (it != m_cache.end())
		return it->second;

	auto texRes = m_texProvider->CreateTextureResource(desc);
	if (!texRes) return TextureHandle::Invalid();

	TextureEntry entry;
	entry.texRes = move(texRes);
	entry.state = LoadState::Pending;

	auto handle = m_loadedTextures.Emplace(move(entry));
	m_cache[key] = handle;

	auto& resID = desc.resID;
	auto resType = resID.GetType();
	switch (resType)
	{
	case Core::ResourceIDType::Path:
	{
		auto requestID = m_assetPipeline->PushRequest(MakeAssetRequest<TextureAsset>(resID));
		m_cpuPending.push_back({ handle, requestID });
	}
	break;
	case Core::ResourceIDType::Runtime:
	case Core::ResourceIDType::Builtin:
	{
		if (!asset) return TextureHandle::Invalid();
		m_gpuPending.push_back({ handle, asset });
	}
	break;
	default:
		return TextureHandle::Invalid();
	}

	return handle;
}

void TextureRepository::Update()
{
	ProcessCpuPending();
	ProcessGpuPending();
	ProcessLoading();
}

void TextureRepository::ProcessCpuPending()
{
	if (m_cpuPending.empty()) return;

	for (auto it = m_cpuPending.begin(); it != m_cpuPending.end(); )
	{
		auto& req = *it;
		auto asset = m_assetPipeline->TakeResult(req.requestId);
		if (!asset.has_value())
		{
			++it;
			continue;
		}

		GpuPendingTextureRequest gpuReq;
		gpuReq.handle = req.handle;
		gpuReq.texAsset = std::static_pointer_cast<TextureAsset>(*asset);
		m_gpuPending.push_back(std::move(gpuReq));

		it = m_cpuPending.erase(it);
	}
}

void TextureRepository::ProcessGpuPending()
{
	for(auto& work : m_gpuPending)
	{
		auto entry = m_loadedTextures.Find(work.handle);
		if (!entry || !entry->texRes) continue;
		if (entry->state != LoadState::Pending) continue; // 중복으로 들어온 경우 이미 Loading/Ready 라면 처리안함.

		if (!m_texProvider->LoadFromAsset(entry->texRes, work.texAsset))
		{
			entry->state = LoadState::Failed;
			continue;
		}
		entry->state = LoadState::GpuLoading;

		m_loadingList.push_back(work.handle);
	}

	m_gpuPending.clear();
}

void TextureRepository::ProcessLoading()
{
	for (auto it = m_loadingList.begin(); it != m_loadingList.end(); )
	{
		auto entry = m_loadedTextures.Find(*it);
		if (!entry || !entry->texRes)
		{
			it = m_loadingList.erase(it);
			continue;
		}

		auto& tex = entry->texRes;
		if (tex->IsReady())
		{
			entry->state = LoadState::Ready;
			it = m_loadingList.erase(it);
		}
		else
			++it;
	}
}

bool TextureRepository::Release(TextureHandle h)
{
	auto entry = m_loadedTextures.Find(h);
	if (!entry) return false;

	for (auto it = m_cache.begin(); it != m_cache.end();)
	{
		if (it->second == h)
			it = m_cache.erase(it);
		else
			++it;
	}
	std::erase(m_loadingList, h);
	return m_loadedTextures.Remove(h);
}

void TextureRepository::ReleaseAll()
{
	m_cpuPending.clear();
	m_gpuPending.clear();
	m_loadingList.clear();

	m_cache.clear();
	m_loadedTextures.Clear();
}