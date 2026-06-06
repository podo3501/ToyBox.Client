#include "pch.h"
#include "GridMaterialResource.h"

GridMaterialResource::~GridMaterialResource() = default;
GridMaterialResource::GridMaterialResource(const MaterialDesc& desc) :
    MaterialResource{ 0 } //텍스쳐를 쓰지 않는다.
{
    Assert(desc.domain == MaterialDomain::Surface);
    m_desc = static_cast<const GridMaterialDesc&>(desc);
}