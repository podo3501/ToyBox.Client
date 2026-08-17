#include "pch.h"
#include "ProviderBudget.h"

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