#include "pch.h"
#include "ResourceProviders.h"

ResourceProviders::~ResourceProviders() = default;
ResourceProviders::ResourceProviders(
	Device& device, 
	DescriptorFactory& descFactory,
	ResourceFactory& resFactory,
	TaskScheduler& taskScheduler) :
	m_device{ device },
	m_texProvider{ m_device, descFactory, taskScheduler, resFactory },
	m_meshProvider{ descFactory, taskScheduler, resFactory },
	m_matProvider{ m_texProvider }
{}

bool ResourceProviders::Initialize(ShaderLibrary& shaderLibrary)
{
	return m_texProvider.Initialize(shaderLibrary);
}

static size_t ComputeTextureBudget(float gpuMs)
{
    size_t baseBudget = 8 * 1024 * 1024;

    if (gpuMs > 10.0f)
        baseBudget = size_t(baseBudget * 0.7f);
    else if (gpuMs < 5.0f)
        baseBudget = size_t(baseBudget * 1.2f);

    return std::clamp<size_t>(
        baseBudget,
        4 * 1024 * 1024,
        32 * 1024 * 1024
    );
}

static size_t ComputeMeshBudget(float gpuMs)
{
    size_t baseBudget = 4 * 1024 * 1024; // 4MB

    if (gpuMs > 10.0f)
        baseBudget = size_t(baseBudget * 0.8f);
    else if (gpuMs < 5.0f)
        baseBudget = size_t(baseBudget * 1.25f);

    return std::clamp<size_t>(
        baseBudget,
        2 * 1024 * 1024,   // min 2MB
        16 * 1024 * 1024   // max 16MB
    );
}

void ResourceProviders::Update(float gpuMs)
{
    m_texProvider.Update(ComputeTextureBudget(gpuMs));
    m_matProvider.Update();
    m_meshProvider.Update(ComputeMeshBudget(gpuMs));
}

