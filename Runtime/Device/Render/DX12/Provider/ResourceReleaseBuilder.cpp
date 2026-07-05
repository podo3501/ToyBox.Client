#include "pch.h"
#include "ResourceReleaseBuilder.h"

ResourceReleaseBuilder::~ResourceReleaseBuilder() = default;
ResourceReleaseBuilder::ResourceReleaseBuilder(TaskScheduler& taskScheduler) noexcept :
	m_taskScheduler{ taskScheduler }
{}