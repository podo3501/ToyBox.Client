#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"
#include "Core/Foundation/Geometry2D.h"

class Device;
class DescriptorFactory;
class RGRenderIDAllocator;

class ViewTargetResource : public IResource
{
public:
    virtual ~ViewTargetResource() override;
    ViewTargetResource();
    virtual bool IsReady() const noexcept override { return m_ready; }
    bool Initialize(
        Device& device, 
        DescriptorFactory& descFactory, 
        RGRenderIDAllocator& idAllocator,
        const Size& size);

    RGResourceID GetColorID() const noexcept { return m_colorID; }
    RGResourceID GetDepthID() const noexcept { return m_depthID; }
    UINT GetColorRTVIndex() const noexcept { return m_colorRTVIndex; }
    UINT GetDepthDSVIndex() const noexcept { return m_depthDSVIndex; }
    UINT GetHeapIndex() const noexcept { return m_heapIndex; }
    const Size& GetSize() const noexcept { return m_size; }
    const Resource& GetColorResource() const noexcept { return m_color; }
    const Resource& GetDepthResource() const noexcept { return m_depth; }

private:
    DescriptorFactory* m_descFactory{ nullptr };
    RGRenderIDAllocator* m_idAllocator{ nullptr };
    bool m_ready{ false };

    Resource m_color;
    Resource m_depth;
    RGResourceID m_colorID;
    RGResourceID m_depthID;
    UINT m_colorRTVIndex{ UINT_MAX };
    UINT m_depthDSVIndex{ UINT_MAX };
    UINT m_heapIndex{ UINT_MAX }; //bindless
    Size m_size;
};