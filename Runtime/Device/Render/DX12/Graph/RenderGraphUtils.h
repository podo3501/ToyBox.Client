#pragma once
#include "RenderPassV.h"
#include "RenderGraphDefinitions.h"

std::vector<PassIndex> TopologicalSort(const std::vector<PassNodeV>& graph);

template <typename T>
static void RemoveVectorDuplicates(std::vector<T>& vec)
{
    std::ranges::sort(vec);
    auto [first, last] = std::ranges::unique(vec); // std::ranges::unique는 중복이 제거된 뒤 남은 '지워야 할 구간'을 subrange로 반환함

    vec.erase(first, last);
}
