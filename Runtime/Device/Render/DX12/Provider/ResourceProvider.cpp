#include "pch.h"
#include "ResourceProvider.h"
#include "Resource/IPendingResource.h"

ResourceProvider::~ResourceProvider() = default;
ResourceProvider::ResourceProvider(TaskScheduler& taskScheduler) noexcept :
	m_releaseBuilder{ taskScheduler }
{}

void ResourceProvider::ReleaseResource(std::shared_ptr<IResource> resource)
{
	if (!resource)
		return;

	m_pendingReleases.emplace_back(std::move(resource));
}

void ResourceProvider::FlushPendingLoad()
{
	for (auto it = m_pendingLoads.begin(); it != m_pendingLoads.end();)
	{
		auto& loadRes = *it;
		if (!loadRes->IsDependencyReady())
		{
			++it;
			continue;
		}
		loadRes->MarkReady();
		it = m_pendingLoads.erase(it);
	}
}

void ResourceProvider::FlushPendingRelease()
{
	if (m_pendingReleases.empty())
		return;

	m_releaseBuilder.ReleaseResources(std::move(m_pendingReleases));
}