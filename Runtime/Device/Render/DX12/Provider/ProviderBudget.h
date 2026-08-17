#pragma once

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

size_t ComputeBudget(float gpuMs, const BudgetRange& range);

namespace ProviderBudget
{
    constexpr BudgetRange Texture
    {
        .fastGpuMs = 5.0f,
        .slowGpuMs = 15.0f,
        .maxBudget = 32_MB,
        .minBudget = 4_MB
    };

    constexpr BudgetRange TextureCube
    {
        .fastGpuMs = 5.0f,
        .slowGpuMs = 15.0f,
        .maxBudget = 16_MB,
        .minBudget = 2_MB
    };

    constexpr BudgetRange Mesh
    {
        .fastGpuMs = 5.0f,
        .slowGpuMs = 15.0f,
        .maxBudget = 16_MB,
        .minBudget = 2_MB
    };
}