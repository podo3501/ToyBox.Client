#include "pch.h"
#include "ResourceProviderSet.h"

ResourceProviderSet::~ResourceProviderSet() = default;
ResourceProviderSet::ResourceProviderSet(
    Device& device,
    TaskScheduler& taskScheduler,
    ResourceFactory& resFactory,
    DescriptorFactory& descFactory) :
    m_device{ device },
    m_pendingRelease{ taskScheduler },
    m_meshProvider{ 
        m_pendingRelease,
        MeshCreateGraphBuilder{ taskScheduler, resFactory, descFactory }
        },
    m_texProvider{
        m_device, 
        taskScheduler, 
        resFactory, 
        descFactory
        },
    m_matProvider{ 
        m_pendingLoad,
        m_pendingRelease,
        m_texProvider 
        },
    m_brushProvider{
        m_pendingLoad,
        m_pendingRelease,
        m_texProvider
    },
    m_cubeProvider{
        TextureCubeCreateGraphBuilder{ taskScheduler, resFactory, descFactory }
        },
    m_envProvider{
        m_pendingLoad,
        m_pendingRelease,
        m_cubeProvider
        }
{
    m_providers[Core::ToIndex(ProviderType::Font)] = &m_fontProvider;
    m_providers[Core::ToIndex(ProviderType::Mesh)] = &m_meshProvider;
    m_providers[Core::ToIndex(ProviderType::Material)] = &m_matProvider;
    m_providers[Core::ToIndex(ProviderType::DebugMaterial)] = &m_debugMatProvider;
    m_providers[Core::ToIndex(ProviderType::Brush)] = &m_brushProvider;
    m_providers[Core::ToIndex(ProviderType::Environment)] = &m_envProvider;

    m_updatables =
    {
        &m_meshProvider,
        &m_texProvider,
        &m_cubeProvider
    };
}

bool ResourceProviderSet::Initialize(ShaderLibrary& shaderLibrary)
{
	return m_texProvider.Initialize(shaderLibrary);
}

void ResourceProviderSet::Update(float gpuMs)
{
    constexpr float SmoothingFactor = 0.1f;

    if (m_avgGpuMs <= 0.0f)
        m_avgGpuMs = gpuMs;
    else
        m_avgGpuMs = std::lerp(m_avgGpuMs, gpuMs, SmoothingFactor);

    for (IUpdatableProvider* provider : m_updatables)
        provider->Update(m_avgGpuMs);

    m_pendingLoad.Flush();
    m_pendingRelease.Flush();
}

