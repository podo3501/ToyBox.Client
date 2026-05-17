#include "pch.h"
#include "MeshRepository.h"
#include "IMeshSystem.h"

struct MeshPendingRequest
{
	ResourceKey key;
	std::shared_ptr<MeshAsset> asset;
	function<std::shared_ptr<MeshAsset>(const filesystem::path&)> loader;
	MeshHandle handle;
};

MeshRepository::~MeshRepository() { ReleaseAll(); }
MeshRepository::MeshRepository(IMeshSystem* meshSystem) :
	m_meshSystem{ meshSystem }
{}

MeshHandle MeshRepository::GetOrCreate(const filesystem::path& path, function<shared_ptr <MeshAsset >(const filesystem::path&) > loader)
{
	ResourceKey key{ path.string(), ResourceKey::Type::File };

	auto it = m_cache.find(key);
	if (it != m_cache.end())
		return it->second;

	auto meshRes = m_meshSystem->CreateMeshResource();
	if (!meshRes) return MeshHandle::Invalid();

	MeshEntry entry;
	entry.path = path;
	entry.meshRes = move(meshRes);
	entry.state = LoadState::Pending;

	auto handle = m_loadedMeshes.Emplace(move(entry));
	m_cache[key] = handle;
	m_pending.push_back(MeshPendingRequest{ key, nullptr, loader, handle });

	return handle;
}

MeshHandle MeshRepository::GetOrCreate(const std::string& runtimeKey, std::shared_ptr<MeshAsset> asset)
{
	ResourceKey key{ runtimeKey, ResourceKey::Type::Runtime };

	auto it = m_cache.find(key);
	if (it != m_cache.end())
		return it->second;

	auto meshRes = m_meshSystem->CreateMeshResource();
	if (!meshRes)
		return MeshHandle::Invalid();

	MeshEntry entry;
	entry.meshRes = std::move(meshRes);
	entry.state = LoadState::Pending;

	auto handle = m_loadedMeshes.Emplace(std::move(entry));
	m_cache[key] = handle;
	m_pending.push_back(MeshPendingRequest{ key, asset, nullptr, handle });

	return handle;
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

		std::shared_ptr<MeshAsset> asset = req.asset;
		if (!asset)
		{
			asset = req.loader(req.key.id);
			if (!asset)
			{
				entry->state = LoadState::Failed;
				continue;
			}
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

void MeshRepository::ReleaseAll()
{
	m_pending.clear();
	m_loadingList.clear();

	m_cache.clear();
	m_loadedMeshes.Clear();
}