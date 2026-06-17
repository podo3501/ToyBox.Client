#pragma once
#include "Resource.h"

class ResourceFactory;
class DescriptorFactory;

class ShadowResource
{
public:
    ShadowResource() = default;
    ~ShadowResource() = default;

    bool Initialize(ResourceFactory& resFactory, DescriptorFactory& factory, UINT width, UINT height);

    const Resource& GetResource() const { return m_resource; }
    Resource& GetResource() { return m_resource; }

    UINT GetDsvIndex() const { return m_dsvIndex; } // 쓰기용 인덱스
    UINT GetSrvIndex() const { return m_srvIndex; } // 읽기용 Bindless 인덱스

private:
    Resource m_resource;

    UINT m_dsvIndex{ UINT_MAX };
    UINT m_srvIndex{ UINT_MAX };
};