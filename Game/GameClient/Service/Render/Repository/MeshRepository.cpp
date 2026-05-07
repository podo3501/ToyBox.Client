#include "pch.h"
#include "MeshRepository.h"
#include "IMeshSystem.h"

struct MeshPendingRequest
{
	filesystem::path path;
	function<std::shared_ptr<MeshAsset>(const filesystem::path&)> loader;
	MeshHandle handle;
};

MeshRepository::~MeshRepository() = default;
MeshRepository::MeshRepository(IMeshSystem* meshSystem) :
	m_meshSystem{ meshSystem }
{}

MeshHandle MeshRepository::GetOrCreate(const filesystem::path& path, function<shared_ptr <MeshAsset >(const filesystem::path&) > loader)
{
	auto it = m_cache.find(path);
	if (it != m_cache.end())
		return it->second;

	auto meshRes = m_meshSystem->CreateMeshResource();
	if (!meshRes) return MeshHandle::Invalid();

	MeshEntry entry;
	entry.path = path;
	entry.meshRes = move(meshRes);
	entry.state = LoadState::Pending;

	auto handle = m_loadedMeshes.Emplace(move(entry));
	m_cache[path] = handle;
	m_pending.push_back(MeshPendingRequest{ path, loader, handle });

	return handle;
}

bool MeshRepository::Release(MeshHandle h)
{
	auto entry = m_loadedMeshes.Find(h);
	if (!entry) return false;

	m_cache.erase(entry->path);
	std::erase(m_loadingList, h);
	return m_loadedMeshes.Remove(h);
}

void MeshRepository::Update()
{
	ProcessPending();
	ProcessLoading();
}

void MeshRepository::ProcessPending()
{
	for (auto& req : m_pending)
	{
		auto entry = m_loadedMeshes.Find(req.handle);
		if (!entry) continue;
		if (entry->state != LoadState::Pending) continue; // 중복으로 들어온 경우 이미 Loading/Ready 라면 처리안함.

		auto asset = req.loader(req.path);
		if (!asset)
		{
			entry->state = LoadState::Failed;
			continue;
		}

		entry->state = LoadState::CpuLoading;

		auto& meshRes = entry->meshRes;
		if (!m_meshSystem->LoadFromAsset(meshRes, asset))
		{
			entry->state = LoadState::Failed;
			continue;
		}

		m_loadingList.push_back(req.handle);
	}

	m_pending.clear();
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