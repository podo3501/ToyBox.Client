#pragma once
#include "Pipeline/Renderer/PipelineCache.h"
#include "InspectorImageRenderer.h"

struct Size;
class Device;
class ShaderLibrary;

class InspectorRenderers
{
public:
    ~InspectorRenderers();
    InspectorRenderers(Device& device, ShaderLibrary& shaderLibaray);
    bool Initialize(const Size& screenSize);
    void SetScreenSize(const Size& screenSize);

    InspectorImageRenderer& GetInspectorImageRenderer() { return m_imageRenderer; }

private:
    Device& m_device;
    PipelineCache m_pipelineCache;

    InspectorImageRenderer m_imageRenderer;
};