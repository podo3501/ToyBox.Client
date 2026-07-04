#include "pch.h"
#include "MeshRepository.h"
#include "IMeshProvider.h"
#include "Service/Render/Desc/MeshDesc.h"
#include "Service/IAssetAsyncLoader.h"
#include "Service/AssetAsyncHelper.h"

struct CpuPendingMeshRequest
{
	MeshHandle handle;
	AssetRequestID requestId;
};

struct GpuPendingMeshRequest
{
	MeshHandle handle;
	std::shared_ptr<MeshAsset> meshAsset;
};

MeshRepository::~MeshRepository() { ReleaseAll(); }
MeshRepository::MeshRepository(IMeshProvider* meshProvider, IAssetAsyncLoader* asyncLoader) :
	m_meshProvider{ meshProvider },
	m_asyncLoader{ asyncLoader }
{}

MeshHandle MeshRepository::GetOrCreate(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset)
{
	auto& resID = desc.resID;
	auto it = m_cache.find(resID);
	if (it != m_cache.end())
		return it->second;

	auto meshRes = m_meshProvider->CreateResource();
	if (!meshRes) return MeshHandle::Invalid();

	MeshEntry entry;
	entry.meshRes = move(meshRes);
	entry.state = LoadState::Pending;

	auto handle = m_loadedMeshes.Emplace(move(entry));
	m_cache[resID] = handle;

	auto resType = resID.GetType();
	switch (resType)
	{
	case Core::ResourceIDType::Path:
	{
		auto reqID = Asset::PushRequest<MeshAsset>(m_asyncLoader, resID);
		m_cpuPending.push_back({ handle, reqID });
	}
	break;
	case Core::ResourceIDType::Runtime:
	case Core::ResourceIDType::Builtin:
	{
		if(!asset) return MeshHandle::Invalid();
		m_gpuPending.push_back({ handle, asset });
	}
	break;
	default:
		return MeshHandle::Invalid();
	}

	return handle;
}

void MeshRepository::ProcessCpuPending()
{
	if (m_cpuPending.empty()) return;

	for (auto it = m_cpuPending.begin(); it != m_cpuPending.end(); )
	{
		auto& req = *it;
		auto asset = m_asyncLoader->TakeResult(req.requestId);
		if (!asset)
		{
			++it;
			continue;
		}

		GpuPendingMeshRequest gpuReq;
		gpuReq.handle = req.handle;
		gpuReq.meshAsset = std::static_pointer_cast<MeshAsset>(asset);
		m_gpuPending.push_back(std::move(gpuReq));

		it = m_cpuPending.erase(it);
	}
}

void MeshRepository::ProcessGpuPending()
{
	for (auto& work : m_gpuPending)
	{
		auto entry = m_loadedMeshes.Find(work.handle);
		if (!entry || !entry->meshRes) continue;
		if (entry->state != LoadState::Pending) continue; // 중복으로 들어온 경우 이미 Loading/Ready 라면 처리안함.

		if (!m_meshProvider->LoadResource(entry->meshRes, work.meshAsset))
		{
			entry->state = LoadState::Failed;
			continue;
		}
		entry->state = LoadState::GpuLoading;

		m_loadingList.push_back(work.handle);
	}

	m_gpuPending.clear();
}

bool MeshRepository::Release(MeshHandle h)
{
	auto entry = m_loadedMeshes.Find(h);
	if (!entry) return false;

	for (auto it = m_cache.begin(); it != m_cache.end();)
	{
		if (it->second == h)
			it = m_cache.erase(it);
		else
			++it;
	}
	std::erase(m_loadingList, h);

	m_meshProvider->ReleaseResource(entry->meshRes);
	return m_loadedMeshes.Remove(h);
}

//Release 함수구현
//지우라고 하면 일단 m_loadedMeshes 여기서 제거를 하고
//meshProvider(backend 레이어에 있는)에 지우라고 하면
//meshProvider는 큐에 넣어놓고 지울 타이밍을 upload 하면서 찾는다.
//meshProvider에서 update 하면서 삭제함.

void MeshRepository::Update()
{
	ProcessCpuPending();
	ProcessGpuPending();
	ProcessLoading();
}

void MeshRepository::ProcessLoading()
{
	for (auto it = m_loadingList.begin(); it != m_loadingList.end(); )
	{
		auto entry = m_loadedMeshes.Find(*it);
		if (!entry || !entry->meshRes)
		{
			it = m_loadingList.erase(it);
			continue;
		}

		auto& mesh = entry->meshRes;
		if (mesh->IsReady())
		{
			entry->state = LoadState::Ready;
			it = m_loadingList.erase(it);
		}
		else
			++it;
	}
}

void MeshRepository::ReleaseAll()
{
	m_cpuPending.clear();
	m_gpuPending.clear();
	m_loadingList.clear();

	m_cache.clear();
	m_loadedMeshes.Clear();
}