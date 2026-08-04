#include "pch.h"
#include "ResourceProvider.h"

namespace
{
    constexpr size_t operator"" _MB(unsigned long long value)
    {
        return value * 1024ull * 1024ull;
    }

    struct BudgetRange
    {
        float fastGpuMs; // 최대 예산을 적용하는 GPU 시간
        float slowGpuMs; // 최소 예산을 적용하는 GPU 시간
        size_t maxBudget; // fastGpuMs 이하일 때의 예산
        size_t minBudget; // slowGpuMs 이상일 때의 예산
    };

    constexpr BudgetRange TextureBudgetRange
    {
        .fastGpuMs = 5.0f,
        .slowGpuMs = 15.0f,
        .maxBudget = 32_MB,
        .minBudget = 4_MB
    };

    constexpr BudgetRange CubeTextureBudgetRange
    {
        .fastGpuMs = 5.0f,
        .slowGpuMs = 15.0f,
        .maxBudget = 16_MB,
        .minBudget = 2_MB
    };

    constexpr BudgetRange MeshBudgetRange
    {
        .fastGpuMs = 5.0f,
        .slowGpuMs = 15.0f,
        .maxBudget = 16_MB,
        .minBudget = 2_MB
    };

    size_t ComputeBudget(
        float gpuMs,
        const BudgetRange& range)
    {
        const float t = std::clamp(
            (gpuMs - range.fastGpuMs) /
            (range.slowGpuMs - range.fastGpuMs),
            0.0f,
            1.0f);

        return static_cast<size_t>(
            std::lerp(
                static_cast<float>(range.maxBudget),
                static_cast<float>(range.minBudget),
                t));
    }

    size_t ComputeTextureBudget(float gpuMs) { return ComputeBudget(gpuMs, TextureBudgetRange); }
    size_t ComputeCubeTextureBudget(float gpuMs) { return ComputeBudget(gpuMs, CubeTextureBudgetRange); }
    size_t ComputeMeshBudget(float gpuMs) { return ComputeBudget(gpuMs, MeshBudgetRange); }
}

ResourceProvider::~ResourceProvider() = default;
ResourceProvider::ResourceProvider(
    Device& device,
    TaskScheduler& taskScheduler,
    ResourceFactory& resFactory,
    DescriptorFactory& descFactory) :
    m_device{ device },
    m_meshProvider{ 
        MeshCreateGraphBuilder{ taskScheduler, resFactory, descFactory },
        ResourceReleaseBuilder{ taskScheduler }
        },
    m_texProvider{
        TextureCreateGraphBuilder{ m_device, taskScheduler, resFactory, descFactory }
        },
    m_matProvider{ 
        m_texProvider, 
        ResourceReleaseBuilder{ taskScheduler }, 
        },
    m_cubeProvider{
        TextureCubeCreateGraphBuilder{ taskScheduler, resFactory, descFactory }
        },
    m_envProvider{
        m_cubeProvider,
        ResourceReleaseBuilder{ taskScheduler },
        }
{}

bool ResourceProvider::Initialize(ShaderLibrary& shaderLibrary)
{
	return m_texProvider.Initialize(shaderLibrary);
}

void ResourceProvider::Update(float gpuMs)
{
    constexpr float SmoothingFactor = 0.1f;

    if (m_avgGpuMs <= 0.0f)
        m_avgGpuMs = gpuMs;
    else
        m_avgGpuMs = std::lerp(m_avgGpuMs, gpuMs, SmoothingFactor);

    m_texProvider.Update(ComputeTextureBudget(m_avgGpuMs));
    m_matProvider.Update();

    m_cubeProvider.Update(ComputeCubeTextureBudget(m_avgGpuMs));
    m_envProvider.Update();

    m_meshProvider.Update(ComputeMeshBudget(m_avgGpuMs));
}

