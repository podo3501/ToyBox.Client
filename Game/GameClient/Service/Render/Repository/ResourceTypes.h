#pragma once

enum class LoadState
{
    Pending,
    CpuLoading,
    GpuLoading,
    Ready,
    Failed
};