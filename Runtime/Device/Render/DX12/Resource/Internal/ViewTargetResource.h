#pragma once
#include "../Resource.h"
#include "Graph/RGTypes.h"
#include "Core/Foundation/Geometry2D.h"
#include <unordered_set>

class Device;
class DescriptorFactory;

struct ViewTarget
{
    Resource color;
    Resource depth;
    RGResourceID colorID;
    RGResourceID depthID;
    UINT heapIndex{ UINT_MAX };
    Size size;
};

class ViewTargetResource
{
public:
    ~ViewTargetResource();
    ViewTargetResource() = delete;
    ViewTargetResource(Device& device, DescriptorFactory& descFactory);

    ViewTarget& Acquire(uint32_t id, const Size& requiredSize); // 뷰 슬롯 기준으로 캐싱된 타겟을 반환, 없거나 크기가 다르면 새로 생성
    void PruneUnused(const std::unordered_set<uint32_t>& activeViews);

private:
    Resource CreateColorTarget(const Size& size);
    Resource CreateDepthTarget(const Size& size);

    Device& m_device;
    DescriptorFactory& m_descFactory;

    std::unordered_map<uint32_t, ViewTarget> m_targets;
};