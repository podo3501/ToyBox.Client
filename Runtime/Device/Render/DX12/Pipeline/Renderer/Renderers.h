#pragma once
#include "PipelineCache.h"
#include "ShadowRenderer.h"
#include "SurfaceRenderer.h"
#include "DebugSurfaceRenderer.h"
#include "UIRenderer.h"
#include "RendererConfig.h"
#include <d3d12.h>

struct Size;
class Device;
class ShaderLibrary;

class Renderers
{
public:
    ~Renderers();
    Renderers(Device& device, ShaderLibrary& shaderLibaray);
    bool Initialize(const Size& screenSize);
    void SetScreenSize(const Size& screenSize);

    ShadowRenderer& GetShadowRenderer() { return m_shadowRenderer; }
    SurfaceRenderer& GetSurfRenderer() { return m_surfRenderer; }
    DebugSurfaceRenderer& GetDebugSurfRenderer() { return m_debugSurfRenderer; }
    UIRenderer& GetUIRenderer() { return m_uiRenderer; }

private:
    Device& m_device;
    RendererConfig m_config;
    PipelineCache m_pipelineCache;

    ShadowRenderer m_shadowRenderer;
    SurfaceRenderer m_surfRenderer;
    DebugSurfaceRenderer m_debugSurfRenderer;
    UIRenderer m_uiRenderer;
};