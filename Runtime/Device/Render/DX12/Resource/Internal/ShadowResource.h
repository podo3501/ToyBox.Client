#pragma once
#include "../Resource.h"

struct Size;
class Device;
class ResourceFactory;
class DescriptorFactory;

class ShadowResource
{
public:
    ~ShadowResource();
    ShadowResource();
    bool Initialize(Device& device, DescriptorFactory& factory, const Size& shadowMapSize);

    const Resource& GetResource() const { return m_resource; }
    Resource& GetResource() { return m_resource; }

    UINT GetDSVIndex() const { return m_dsvIndex; } // 쓰기용 인덱스
    UINT GetSRVIndex() const { return m_srvIndex; } // 읽기용 Bindless 인덱스

private:
    Resource m_resource;
    UINT m_dsvIndex{ UINT_MAX };
    UINT m_srvIndex{ UINT_MAX };
};